//------------------------------------------------------------------------------
/*! \file xfblib.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2015 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief Fire behavior C API library.
 *
 *  This file is a collection of fire behavior functions containing the
 *  algorithms used by BehavePlus.
 *
 *  Complex data structures are eschewed as function parameters;
 *  all parameters are passed as scalers or simple arrays,
 *  never as pointers to typedefs or classes.
 *  This allows the API to be called by higher level functions with their
 *  own notion of data structures or to be wrapped into C++ classes.
 */

// Custom include files
#include "Bp6Globals.h"
#include "xfblib.h"
#include "Bp6CrownFire.h"
#include "Bp6SurfaceFire.h"

// Standard include files
#include <stdio.h>
#include <stdlib.h>

#ifdef INCLUDE_V5_CODE
//------------------------------------------------------------------------------
//  This bunch of fuel bed intermediate variables are derived in
//  FBL_FuelBedIntermediates() but used in FBL_FuelBedHeatSink(),
//  FBL_SurfaceFireReactionIntensity(), or FBL_SurfaceFireSpreadAtHead().
//------------------------------------------------------------------------------

const int MAX_PARTS    = 8;         //!< Maximum number of fuel particles.
const int MAX_CATS     = 2;         //!< Life categories
const int MAX_SIZES    = 6;         //!< Fuel moisture time lag classes.
const int DEAD_CAT     = 0;         //!< Dead life category index
const int LIVE_CAT     = 1;         //!< Live life category index

// Set in FBL_FuelBedIntermediates(), used in FBL_SurfaceFuelBedHeatSink()
static int    m_particles;          //!< Number of fuel particles
static int    m_life[MAX_PARTS];    //!< Fuel particle life category
static double m_aWtg[MAX_PARTS];    //!< Fuel particle area weighting factor
static double m_load[MAX_PARTS];    //!< Fuel particle fuel load (lb/ft2)
static double m_sigK[MAX_PARTS];    //!< Fuel particle surface area-to-volume ratio (ft2/ft3)
static double m_lifeAwtg[MAX_CATS]; //!< Life category weighting factor
static double m_lifeFine[MAX_CATS]; //!< Fine fuel ratio by life category
static double m_liveMextK;          //!< Live moisture of extinction constant

// Set in FBL_FuelBedIntermediates(), used in FBL_SurfaceFireReactionIntensity().
static double m_lifeRxK[MAX_CATS];  //!< Reaction intensity constant by life category

// Set in FBL_SurfaceFuelBedIntermediates(), used in FBL_SurfaceFireForwardSpreadRate().
static double m_slopeK;             //!< Slope constant K (see Rothermel 1972)
// Set in FBL_SurfaceFuelBedIntermediates(), used in FBL_SurfaceFireForwardSpreadRate()
// and in FBL_SurfaceFireEffectiveWindSpeedAtVector()
static double m_windB;              //!< Wind constant B (see Rothermel 1972)
static double m_windE;              //!< Wind constant E (see Rothermel 1972)
static double m_windK;              //!< Wind constant K (see Rothermel 1972)
#endif

//------------------------------------------------------------------------------
//  FOFEM tree species and equations
//  These are used in the bark thickness and tree mortality functions.
//  Note: Region 1=Interior West, 2=Pacific West, 3=NorthEast, 4=SouthEast.
//
//  NOTE: FOFEM v6 introduced new species codes for all species, and also
//	introduced 13 new species and dropped 2 other species.
//	The FOFEM 6 abbreviations are under structure member named 'fofem6', while
//  the FOFEM 5 abbreviations are under structure member named 'fofem5' (was 'abbrev').
//  The order of the original FOFEM 5 species indices have not changed,
//	since those are hard-wired referenced into client code.
//
//  NOTE: Through BP5, there were only mortality equations 1 and 3.
//  With BP6 we introiduce mortality equations 10 through 20.
//------------------------------------------------------------------------------

typedef struct _fofemSpecies
{
	char *fofem6;		// FOFEM 6 genus-species abbreviation
    char *fofem5;       // FOFEM 5 genus-species abbreviation
    int   mortEq;       // Index to mortality equation (base 1): 1, 3, and 10-20
    int   barkEq;       // Index to single bark thickness equation (base 1)
    int   regions;      // Region list (any combination of 1, 2, 3, and/or 4)
    char *scientific;   // Scientific name
    char *common;       // Common name
} FofemSpeciesStruct;

static FofemSpeciesStruct FofemSpecies[] =
{
/* 000 */ { "ABAM",   "ABIAMA",  1, 26,    2, "Abies amabilis",               "Pacific silver fir" },
/* 001 */ { "ABBA",   "ABIBAL",  1, 10,  134, "Abies balsamea",               "Balsam fir" },
/* 002 */ { "ABCO",   "ABICON", 10, 27,   12, "Abies concolor",               "White fir" },
/* 003 */ { "ABGR",   "ABIGRA", 11, 25,   12, "Abies grandis",                "Grand fir" },
/* 004 */ { "ABLA",   "ABILAS", 11, 20,   12, "Abies lasiocarpa",             "Subalpine fir" },
/* 005 */ { "ABMA",   "ABIMAG", 16, 18,   12, "Abies magnifica",              "Red fir" },
/* 006 */ { "ABPR",   "ABIPRO",  1, 24,    2, "Abies procera",                "Noble fir" },
/* 007 */ { "ABISPP", "ABISPP",  1, 30,   34, "Abies species",                "Firs" },
/* 008 */ { "ACBA3",  "ACEBAR",  1,  8,    4, "Acer barbatum",                "Southern sugar maple" },
/* 009 */ { "ACLE",   "ACELEU",  1,  8,    4, "Acer leucoderme",              "Chalk maple" },
/* 010 */ { "ACMA3",  "ACEMAC",  1,  3,    2, "Acer macrophyllum",            "Bigleaf maple" },
/* 011 */ { "ACNE2",  "ACENEG",  1, 13,   34, "Acer negundo",                 "Boxelder" },
/* 012 */ { "ACNI5",  "ACENIG",  1, 14,   34, "Acer nigrum",                  "Black maple" },
/* 013 */ { "ACPE",   "ACEPEN",  1, 24,   34, "Acer pensylvanicum",           "Striped maple" },
/* 014 */ { "ACRU",   "ACERUB",  1,  7,   34, "Acer rubrum",                  "Red maple" },
/* 015 */ { "ACSA2",  "ACESACI", 1, 10,   34, "Acer saccharinum",             "Silver maple" },
/* 016 */ { "ACSA3",  "ACESACU", 1, 12,   34, "Acer saccharum",               "Sugar maple" },
/* 017 */ { "ACESPP", "ACESPI",  1, 19,    3, "Acer spicatum",                "Mountain maple" },
/* 018 */ { "ACSP2",  "ACESPP",  1,  8,   34, "Acer species",                 "Maples" },
/* 019 */ { "AEGL",   "AESGLA",  1, 15,   34, "Aesculus glabra",              "Ohio buckeye" },
/* 020 */ { "AEOC2",  "AESOCT",  1, 29,   34, "Aesculus octandra",            "Yellow buckeye" },
/* 021 */ { "AIAL",   "AILALT",  1, 29,   34, "Ailanthus altissima",          "Ailanthus" },
/* 022 */ { "ALRH2",  "ALNRHO",  1, 35,    2, "Alnus rhombifolia",            "White alder" },
/* 023 */ { "ALRU2",  "ALNRUB",  1,  5,    2, "Alnus rubra",                  "Red alder" },
/* 024 */ { "AMAR3",  "AMEARB",  1, 29,   34, "Amelanchier arborea",          "Common serviceberry" },
/* 025 */ { "ARME",   "ARBMEN",  1, 34,    2, "Arbutus menziesii",            "Pacific madrone" },
/* 026 */ { "BEAL2",  "BETALL",  1, 10,   34, "Betula alleghaniensis",        "Yellow birch" },
/* 027 */ { "BELE",   "BETLEN",  1,  9,    4, "Betula lenta",                 "Sweet birch" },
/* 028 */ { "BENI",   "BETNIG",  1,  8,   34, "Betula nigra",                 "River birch" },
/* 029 */ { "BEOC2",  "BETOCC",  1, 29,   34, "Betula occidentalis",          "Water birch" },
/* 030 */ { "BEPA",   "BETPAP",  1,  6,  234, "Betula papyrifera",            "Paper birch" },
/* 031 */ { "BETSPP", "BETSPP",  1, 12,  234, "Betula species ",              "Birches" },
/* 032 */ { "CEOC",   "CELOCC",  1, 14,   34, "Celtis occidentalis",          "Common hackberry" },
/* 033 */ { "CAAQ2",  "CARAQU",  1, 19,   34, "Carya aquatica",               "Water hickory" },
/* 034 */ { "CACA18", "CARCAR",  1,  9,   34, "Carpinus caroliniana",         "American hornbeam" },
/* 035 */ { "CACOL3", "CARCOR",  1, 16,   34, "Carya cordiformis",            "Bitternut hickory" },
/* 036 */ { "CAGL8",  "CARGLA",  1, 16,   34, "Carya glabra",                 "Pignut hickory" },
/* 037 */ { "CAIL2",  "CARILL",  1, 15,   34, "Carya illinoensis",            "Pecan" },
/* 038 */ { "CALA21", "CARLAC",  1, 22,   34, "Carya laciniosa",              "Shellbark hickory" },
/* 039 */ { "CAOV2",  "CAROVA",  1, 19,   34, "Carya ovata",                  "Shagbark hickory" },
/* 040 */ { "CARSPP", "CARSPP",  1, 23,   34, "Carya species",                "Hickories" },
/* 041 */ { "CATE9",  "CARTEX",  1, 19,    4, "Carya texana",                 "Black hickory" },
/* 042 */ { "CATO6",  "CARTOM",  1, 22,   34, "Carya tomentosa",              "Mockernut hickory" },
/* 043 */ { "CACHM",  "CASCHR",  1, 24,    2, "Castanopsis chrysophylla",     "Giant chinkapin" },
/* 044 */ { "CADE12", "CASDEN",  1, 19,    3, "Castanea dentata",             "American chestnut" },
/* 045 */ { "CATSPP", "CATSPP",  1, 16,    4, "Catalpa species",              "Catalpas" },
/* 046 */ { "CELA",   "CELLAE",  1, 15,   34, "Celtis laevigata",             "Sugarberry" },
/* 047 */ { "CECA4",  "CERCAN",  1, 14,   34, "Cercis canadensis",            "Eastern redbud" },
/* 048 */ { "CHLA",   "CHALAW",  1, 39,    2, "Chamaecyparis lawsoniana",     "Port Orford cedar" },
/* 049 */ { "CHNO",   "CHANOO",  1,  2,    2, "Chamaecyparis nootkatenis",    "Alaska cedar" },
/* 050 */ { "CHTH2",  "CHATHY",  1,  4,   34, "Chamaecyparis thyoides",       "Atlantic white cedar" },
/* 051 */ { "COFL2",  "CORFLO",  1, 20,   34, "Cornus florida",               "Flowering dogwood" },
/* 052 */ { "CONU4",  "CORNUT",  1, 35,    2, "Cornus nuttallii",             "Pacific dogwood" },
/* 053 */ { "CORSPP", "CORSPP",  1, 10,   34, "Cornus species",               "Dogwoods" },
/* 054 */ { "CRDO2",  "CRADOU",  1, 17,    4, "Crataegus douglasii",          "Black hawthorn" },
/* 055 */ { "CRASPP", "CRASPPW", 1, 35,    2, "Crataegus species (western)",  "Hawthorns (western)" },
/* 056 */ { "",       "CRASPPE", 1, 17,   34, "Crataegus species (eastern)",  "Hawthorns (eastern)" },
/* 057 */ { "DIVI5",  "DIOVIR",  1, 20,   34, "Diospyros virginiana",         "Persimmon" },
/* 058 */ { "FAGR",   "FAGGRA",  1,  4,   34, "Fagus grandifolia",            "American beech" },
/* 059 */ { "FRAM2",  "FRAAMA",  1, 21,   34, "Fraxinus americana",           "White ash" },
/* 060 */ { "FRNI",   "FRANIG",  1, 14,   34, "Fraxinus nigra",               "Black ash" },
/* 061 */ { "FRPE",   "FRAPEN",  1, 18,   34, "Fraxinus pennsylvanica",       "Green ash" },
/* 062 */ { "FRPR",   "FRAPRO",  1, 16,   34, "Fraxinus profunda",            "Pumpkin ash" },
/* 063 */ { "FRQU",   "FRAQUA",  1,  9,   34, "Fraxinus quadrangulata",       "Blue ash" },
/* 064 */ { "FRASPP", "FRASPP",  1, 21,   34, "Fraxinus species",             "Ashes" },
/* 065 */ { "GLTR",   "GLETRI",  1, 17,   34, "Gleditsia triacanthos",        "Honeylocust" },
/* 066 */ { "GOLA",   "GORLAS",  1, 17,    4, "Gordonia lasianthus",          "Loblolly bay" },
/* 067 */ { "GYDI",   "GYMDIO",  1, 10,   34, "Gymnocladus dioicus",          "Kentucky coffeetree" },
/* 068 */ { "HALSPP", "HALSPP",  1, 17,    4, "Halesia species",              "Silverbells" },
/* 069 */ { "ILOP",   "ILEOPA",  1, 21,   34, "Ilex opaca",                   "American holly" },
/* 070 */ { "JUCI",   "JUGCIN",  1, 20,   34, "Juglans cinerea",              "Butternut" },
/* 071 */ { "JUNI",   "JUGNIG",  1, 20,   34, "Juglans nigra",                "Black walnut" },
/* 072 */ { "JUOC",   "JUNOCC",  1,  4,    2, "Juniperus occidentalis",       "Western juniper" },
/* 073 */ { "JUNSPP", "JUNSPP",  1, 12,   34, "Juniperus species",            "Junipers/Redcedars" },
/* 074 */ { "JUVI",   "JUNVIR",  1, 17,   34, "Juniperus virginiana",         "Eastern redcedar" },
/* 075 */ { "LALA",   "LARLAR",  1, 10,   34, "Larix laricina",               "Tamarack" },
/* 076 */ { "LALY",   "LARLYA",  1, 29,    2, "Larix lyallii",                "Subalpine larch" },
/* 077 */ { "LAOC",   "LAROCC", 14, 36,   12, "Larix occidentalis",           "Western larch" },
/* 078 */ { "LIDE",   "LIBDEC", 12, 34,    2, "Libocedrus decurrens",         "Incense cedar" },
/* 079 */ { "LIST2",  "LIQSTY",  1, 15,   34, "Liquidambar styraciflua",      "Sweetgum" },
/* 080 */ { "LITU",   "LIRTUL",  1, 20,   34, "Liriodendron tulipifera",      "Yellow poplar" },
/* 081 */ { "LIDE3",  "LITDEN",  1, 30,    2, "Lithocarpus densiflorus",      "Tanoak" },
/* 082 */ { "MAPO",   "MACPOM",  1, 16,    4, "Maclura pomifera",             "Osage orange" },
/* 083 */ { "MAAC",   "MAGACU",  1, 15,   34, "Magnolia acuminata",           "Cucumber tree" },
/* 084 */ { "MAGR4",  "MAGGRA",  1, 12,    4, "Magnolia grandiflora",         "Southern magnolia" },
/* 085 */ { "MAMA2",  "MAGMAC",  1, 12,    4, "Magnolia macrophylla",         "Bigleaf magnolia" },
/* 086 */ { "MAGSPP", "MAGSPP",  1, 18,   34, "Magnolia species",             "Magnolias" },
/* 087 */ { "MAVI2",  "MAGVIR",  1, 19,   34, "Magnolia virginiana",          "Sweetbay" },
/* 088 */ { "MALPRU", "MALPRU",  1, 17,    4, "Prunus species",               "Apples/Cherries" },
/* 089 */ { "MALSPP", "MALSPP",  1, 22,   34, "Malus species",                "Apples" },
/* 090 */ { "MOAL",   "MORALB",  1, 17,    4, "Morus alba",                   "White mulberry" },
/* 091 */ { "MORU2",  "MORRUB",  1, 17,    4, "Morus rubra",                  "Red mulberry" },
/* 092 */ { "MORSPP", "MORSPP",  1, 12,   34, "Morus species",                "Mulberries" },
/* 093 */ { "NYAQ2",  "NYSAQU",  1,  9,    4, "Nyssa aquatica",               "Water tupelo" },
/* 094 */ { "NYOG",   "NYSOGE",  1, 17,    4, "Nyssa ogache",                 "Ogeechee tupelo" },
/* 095 */ { "NYSSPP", "NYSSPP",  1,  4,   34, "Nyssa species",                "Tupelos" },
/* 096 */ { "NYSY",   "NYSSYL",  1, 18,   34, "Nyssa sylvatica",              "Black gum, Black tupelo" },
/* 097 */ { "NYBI",   "NYSSYLB", 1, 16,    4, "Nyssa biflora",                "Swamp tupelo" },
/* 098 */ { "OSVI",   "OSTVIR",  1, 16,   34, "Ostrya virginiana",            "Hophornbeam" },
/* 099 */ { "OXAR",   "OXYARB",  1, 15,   34, "Oxydendrum arboreum",          "Sourwood" },
/* 100 */ { "PATO2",  "PAUTOM",  1, 29,   34, "Paulownia tomentosa",          "Princess tree" },
/* 101 */ { "PEBO",   "PERBOR",  1, 17,    4, "Persea borbonia",              "Redbay" },
/* 102 */ { "PIAB",   "PICABI",  3,  8,   34, "Picea abies",                  "Norway spruce" },
/* 103 */ { "PIEN",   "PICENG", 15, 15,   12, "Picea engelmannii",            "Engelmann spruce" },
/* 104 */ { "PIGL",   "PICGLA",  3,  4,  123, "Picea glauca",                 "White spruce" },
/* 105 */ { "PIMA",   "PICMAR",  3, 11,  234, "Picea mariana",                "Black spruce" },
/* 106 */ { "PIPU",   "PICPUN",  3, 10,    1, "Picea pungens",                "Blue spruce" },
/* 107 */ { "PIRU",   "PICRUB",  3, 13,   34, "Picea rubens",                 "Red spruce" },
/* 108 */ { "PISI",   "PICSIT",  3,  6,    2, "Picea sitchensis",             "Sitka spruce" },
/* 109 */ { "PICSPP", "PICSPP",  3, 13,   34, "Picea species",                "Spruces" },
/* 110 */ { "PIAL",   "PINALB", 17,  9,   12, "Pinus albicaulis",             "Whitebark pine" },
/* 111 */ { "PIAT",   "PINATT",  1,  9,    2, "Pinus attenuata",              "Knobcone pine" },
/* 112 */ { "PIBA2",  "PINBAN",  1, 19,    3, "Pinus banksiana",              "Jack pine" },
/* 113 */ { "PICL",   "PINCLA",  1, 14,    4, "Pinus clausa",                 "Sand pine" },
/* 114 */ { "PICO",   "PINCON", 17,  7,   12, "Pinus contorta",               "Lodgepole pine" },
/* 115 */ { "PIEC2",  "PINECH",  1, 16,   34, "Pinus echinata",               "Shortleaf pine" },
/* 116 */ { "PIEL",   "PINELL",  1, 31,    4, "Pinus elliottii",              "Slash pine" },
/* 117 */ { "PIFL2",  "PINFLE",  1,  9,    1, "Pinus flexilis",               "Limber pine" },
/* 118 */ { "PIGL2",  "PINGLA",  1, 14,    4, "Pinus glabra",                 "Spruce pine" },
/* 119 */ { "PIJE",   "PINJEF", 19, 37,   12, "Pinus jeffreyi",               "Jeffrey pine" },
/* 120 */ { "PILA",   "PINLAM", 18, 38,   12, "Pinus lambertiana",            "Sugar pine" },
/* 121 */ { "PIMO3",  "PINMON",  1, 14,   12, "Pinus monticola",              "Western white pine" },
/* 122 */ { "PIPA2",  "PINPAL",  5, 40,    4, "Pinus palustrus",              "Longleaf pine" },
/* 123 */ { "PIPO",   "PINPON", 19, 36,   12, "Pinus ponderosa",              "Ponderosa pine" },
/* 124 */ { "PIPU5",  "PINPUN",  1, 19,   34, "Pinus pungens",                "Table mountain pine" },
/* 125 */ { "PIRE",   "PINRES",  1, 22,   34, "Pinus resinosa",               "Red pine" },
/* 126 */ { "PIRI",   "PINRIG",  1, 24,   34, "Pinus rigida",                 "Pitch pine" },
/* 127 */ { "PISA2",  "PINSAB",  1, 12,    2, "Pinus sabiniana",              "Gray (Digger) pine" },
/* 128 */ { "PISE",   "PINSER",  1, 35,   34, "Pinus serotina",               "Pond pine" },
/* 129 */ { "PINSPP", "PINSPP",  1,  9,   34, "Pinus species",                "Pines" },
/* 130 */ { "PIST",   "PINSTR",  1, 24,   34, "Pinus strobus",                "Eastern white pine" },
/* 131 */ { "PISY",   "PINSYL",  1,  9,   34, "Pinus sylvestris",             "Scots pine" },
/* 132 */ { "PITA",   "PINTAE",  1, 30,   34, "Pinus taeda",                  "Loblolly pine" },
/* 133 */ { "PIVI2",  "PINVIR",  1, 12,   34, "Pinus virginiana",             "Virginia pine" },
/* 134 */ { "PLOC",   "PLAOCC",  1, 12,   34, "Plantus occidentalis",         "American sycamore" },
/* 135 */ { "POBA2",  "POPBAL",  1, 19,   34, "Populus balsamifera",          "Balsam poplar" },
/* 136 */ { "PODE3",  "POPDEL",  1, 19,   34, "Populus deltoides",            "Eastern cottonwood" },
/* 137 */ { "POGR4",  "POPGRA",  1, 18,   34, "Populus grandidentata",        "Bigtooth aspen" },
/* 138 */ { "POHE4",  "POPHET",  1, 29,   34, "Populus heterophylla",         "Swamp cottonwood" },
/* 139 */ { "POPSPP", "POPSPP",  1, 17,   34, "Populus species",              "Poplars" },
/* 140 */ { "POTR15", "POPTRI",  1, 23,    2, "Populus trichocarpa",          "Black cottonwood" },
/* 141 */ { "PRAM",   "PRUAME",  1, 19,    3, "Prunus americana",             "American plum" },
/* 142 */ { "PREM",   "PRUEMA",  1, 35,    2, "Prunus emarginata",            "Bitter cherry" },
/* 143 */ { "PRPE2",  "PRUDEN",  1, 24,   34, "Prunus pensylvanica",          "Pin cherry" },
/* 144 */ { "PRSE2",  "PRUSER",  1,  9,   34, "Prunus serotina",              "Black cherry" },
/* 145 */ { "",       "PRUSPP",  1, 29,   34, "Prunus species",               "Cherries" },
/* 146 */ { "PRVI",   "PRUVIR",  1, 19,    3, "Prunus virginiana",            "Chokecherry" },
/* 147 */ { "PSME",   "PSEMEN", 20, 36,   12, "Pseudotsuga menziesii",        "Douglas-fir" },
/* 148 */ { "QUAG",   "QUEAGR",  1, 29,    2, "Quercus agrifolia",            "California live oak" },
/* 149 */ { "QUAL",   "QUEALB",  1, 19,   34, "Quercus alba",                 "White oak" },
/* 150 */ { "QUBI",   "QUEBIC",  1, 24,   34, "Quercus bicolor",              "Swamp white oak" },
/* 151 */ { "QUCH2",  "QUECHR",  1,  3,    2, "Quercus chrysolepis",          "Canyon live oak" },
/* 152 */ { "QUOC2",  "QUEOCC",  1, 19,   34, "Quercus coccinea",             "Scarlet oak" },
/* 153 */ { "QUDU",   "QUEDOU",  1, 12,    2, "Quercus douglasii",            "Blue oak" },
/* 154 */ { "QUEL",   "QUEELL",  1, 17,   34, "Quercus ellipsoidalis",        "Northern pin oak" },
/* 155 */ { "QUEN",   "QUEENG",  1, 33,    2, "Quercus engelmannii",          "Engelmann oak" },
/* 156 */ { "QUFA",   "QUEFAL",  1, 23,   34, "Quercus falcata",              "Southern red oak" },
/* 157 */ { "QUGA4",  "QUEGAR",  1,  8,    2, "Quercus garryana",             "Oregon white oak" },
/* 158 */ { "QUIM",   "QUEIMB",  1, 20,   34, "Quercus imbricaria",           "Shingle oak" },
/* 159 */ { "QUIN",   "QUEINC",  1, 17,    4, "Quercus incana",               "Bluejack oak" },
/* 160 */ { "QUKE",   "QUEKEL",  1,  9,    2, "Quercus kellogii",             "Califonia black oak" },
/* 161 */ { "QULA2",  "QUELAE",  1, 16,    4, "Quercus laevis",               "Turkey oak" },
/* 162 */ { "QULA3",  "QUELAU",  1, 15,    4, "Quercus laurifolia",           "Laurel oak" },
/* 163 */ { "QULO",   "QUELOB",  1, 22,    2, "Quercus lobata",               "Valley oak" },
/* 164 */ { "QULY",   "QUELYR",  1, 18,   34, "Quercus lyrata",               "Overcup oak" },
/* 165 */ { "QUMA2",  "QUEMAC",  1, 21,   34, "Quercus macrocarpa",           "Bur oak" },
/* 166 */ { "QUMA3",  "QUEMAR",  1, 16,   34, "Quercus marilandica",          "Blackjack oak" },
/* 167 */ { "QUMI",   "QUEMIC",  1, 25,   34, "Quercus michauxii",            "Swamp chestnut oak" },
/* 168 */ { "QUMU",   "QUEMUE",  1, 21,   34, "Quercus muehlenbergii",        "Chinkapin oak" },
/* 169 */ { "QUNI",   "QUENIG",  1, 15,   34, "Quercus nigra",                "Water oak" },
/* 170 */ { "QUNU",   "QUENUT",  1,  9,    4, "Quercus nuttallii",            "Nuttall oak" },
/* 171 */ { "QUPA2",  "QUEPAL",  1, 20,   34, "Quercus palustris",            "Pin oak" },
/* 172 */ { "QUPH",   "QUEPHE",  1, 20,   34, "Quercus phellos",              "Willow oak" },
/* 173 */ { "QUPR2",  "QUEPRI",  1, 28,   34, "Quercus prinus",               "Chestnut oak" },
/* 174 */ { "QURU",   "QUERUB",  1, 21,   34, "Quercus rubra",                "Northern red oak" },
/* 175 */ { "QUSH",   "QUESHU",  1, 16,   34, "Quercus shumardii",            "Shumard oak" },
/* 176 */ { "QUESPP", "QUESPP",  1, 24,   34, "Quercus species",              "Oaks" },
/* 177 */ { "QUST",   "QUESTE",  1, 23,   34, "Quercus stellata",             "Post oak" },
/* 178 */ { "QUVE",   "QUEVEL",  1, 24,   34, "Quercus velutina",             "Black oak" },
/* 179 */ { "QUVI",   "QUEVIR",  1, 22,    4, "Quercus virginiana",           "Live oak" },
/* 180 */ { "QUWI2",  "QUEWIS",  1, 13,    2, "Quercus wislizenii",           "Interior live oak" },
/* 181 */ { "ROPS",   "ROBPSE",  1, 28,   34, "Robinia pseudoacacia",         "Black locust" },
/* 182 */ { "SABE2",  "SALDIA",  1, 19,    3, "Salix bebbiana",               "Diamond willow" },
/* 183 */ { "SANI",   "SALNIG",  1, 19,   34, "Salix nigra",                  "Black willow" },
/* 184 */ { "SALSPP", "SALSPP",  1, 20,  234, "Salix species",                "Willows" },
/* 185 */ { "SAAL5",  "SASALB",  1, 14,   34, "Sassafras albidum",            "Sassafras" },
/* 186 */ { "SEGI2",  "SEQGIG",  1, 39,    2, "Sequoiadendron gigantea",      "Giant sequoia" },
/* 187 */ { "SESE3",  "SEQSEM",  1, 39,    2, "Sequoia sempervirens",         "Redwood" },
/* 188 */ { "SOAM3",  "SORAME",  1, 19,    3, "Sorbus americana",             "American mountain ash" },
/* 189 */ { "TABR2",  "TAXBRE",  1,  4,   12, "Taxus brevifolia",             "Pacific yew" },
/* 190 */ { "TADI2",  "TAXDIS",  1,  4,   34, "Taxodium distichum",           "Bald cypress" },
/* 191 */ { "TAAS",   "TAXDISN", 1, 21,    4, "Taxodium distictum var. nutans", "Pond cypress" },
/* 192 */ { "THOC2",  "THUOCC",  1,  4,   34, "Thuja occidentalis",           "Northern white cedar" },
/* 193 */ { "THPL",   "THUPLI",  1, 14,   12, "Thuja plicata",                "Western redcedar" },
/* 194 */ { "THUSPP", "THUSPP",  1, 12,   34, "Thuju species",                "Arborvitae" },
/* 195 */ { "TIAM",   "TILAME",  1, 17,   34, "Tilia americana",              "American basswood" },
/* 196 */ { "TIHE",   "TILHET",  1, 29,   34, "Tilia heterophylla",           "White basswood" },
/* 197 */ { "TSCA",   "TSUCAN",  1, 18,   34, "Tsuga canadensis",             "Eastern hemlock" },
/* 198 */ { "TSHE",   "TSUHET",  1, 19,   12, "Tsuga heterophylla",           "Western hemlock" },
/* 199 */ { "TSME",   "TSUMER",  1, 19,   12, "Tsuga mertensiana",            "Mountain hemlock" },
/* 200 */ { "ULAL",   "ULMALA",  1, 10,   34, "Ulmus alata",                  "Winged elm" },
/* 201 */ { "ULAM",   "ULMAME",  1, 10,   34, "Ulmus americana",              "American elm" },
/* 202 */ { "ULPU",   "ULMPUM",  1, 17,   34, "Ulmus pumila",                 "Siberian elm" },
/* 203 */ { "ULRU",   "ULMRUB",  1, 11,   34, "Ulmus rubra",                  "Slippery elm" },
/* 204 */ { "ULMSPP", "ULMSPP",  1, 18,   34, "Ulmus species",                "Elms" },
/* 205 */ { "ULTH",   "ULMTHO",  1, 12,   34, "Ulmus thomasii",               "Rock elm" },
/* 206 */ { "UMCA",   "UMBCAL",  1,  5,    2, "Umbellularia californica",     "California laurel" },

/* 207 */ { "ABLO",    "ABLO",    10, 27,   12, "Abies lowiana",                "Sierra white fir" },
/* 208 */ { "ABNO",    "ABNO",     1, 24,   12, "Abies nobilis",                "Noble fir", },
/* 209 */ { "AEFL",    "AEFL",     1, 29,   34, "Aesculus flava",               "Yellow buckeye" },
/* 210 */ { "CANO9",   "CANO9",    1,  2,    2, "Callitropsis nootkatensis",    "Alaska cedar" },
/* 211 */ { "CADE27",  "CADE27",  12, 34,   12, "Calocedrus decurrens",         "Incense cedar" },
/* 212 */ { "CAAL27",  "CAAL27",   1, 22,   34, "Carya alba",                   "Mockernut hickory" },
/* 213 */ { "CACA38",  "CACA38",   1, 19,   34, "Carya carolinae septentrionalis", "Shagbark hickory" },
/* 214 */ { "CAAM29",  "CAAM29",   1, 19,   34, "Castenea Americana",           "American chestnut" },
/* 215 */ { "CHCHC4",  "CHCHC4",   1, 24,   34, "Chrysolepis chrysophylla",     "Giant chinkapin" },
/* 216 */ { "CUNO",    "CUNO",     1,  2,    2, "Cupressus nootkatensis",       "Nootka cypress" },
/* 217 */ { "CUTH",    "CUTH",     1,  4,    2, "Cupressus thyoides",           "Atlantic white cedar" },
/* 218 */ { "QUTE",    "QUTE",     1,  9,   34, "Quercus texana",               "Texas red oak" },
/* 219 */ { "ULRA",    "ULRA",     1, 12,   34, "Ulmus racemosa",               "Rock elm" },

/* 220 */ {  NULL,      NULL,     0,  0,    0,  NULL,                           NULL }
};

//------------------------------------------------------------------------------
/*! \brief Returns the interpolated/extrapolated value based upon curing.
 *
 *  \param curing   Curing level (fraction)
 *  \param valueArray Array of 6 boundary values.
 *
 *  \return Interpolated value.
 */
double FBL_AspenInterpolate( double curing, double *valueArray )
{
    static double Curing[] = { 0.0, 0.3, 0.5, 0.7, 0.9, 1.000000001 };
    curing = ( curing < 0.0 ) ? 0.0 : curing;
    curing = ( curing > 1.0 ) ? 1.0 : curing;
    double fraction = 0.0;
    unsigned i = 1;
    for ( i=1; i<sizeof( Curing )-1; i++ )
    {
        if ( curing < Curing[i] )
        {
            fraction = 1. - ( Curing[i] - curing ) / ( Curing[i] - Curing[i-1] );
            break;
        }
    }
    double value = valueArray[i-1] + fraction * ( valueArray[i] - valueArray[i-1] );
    return( value );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen fuel bed depth.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen fuel bed depth (ft).
 */

double FBL_AspenFuelBedDepth( int typeIndex, double /* curing */ )
{
    double Depth[] = { 0.65, 0.30, 0.18, 0.50, 0.18 };
    return( Depth[typeIndex] );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen fuel bed depth.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen fuel bed depth (ft).
 */

double FBL_AspenFuelMextDead( int /* typeIndex */, double /* curing */ )
{
    return( 0.25 );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen dead 0.0 - 0.25" load.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen dead 0.0 - 0.25" load (lb/ft2).
 */

double FBL_AspenLoadDead1( int typeIndex, double curing )
{
    static double Load[5][6] = {
        { 0.800, 0.893, 1.056, 1.218, 1.379, 1.4595 },
        { 0.738, 0.930, 1.056, 1.183, 1.309, 1.3720 },
        { 0.601, 0.645, 0.671, 0.699, 0.730, 0.7455 },
        { 0.880, 0.906, 1.037, 1.167, 1.300, 1.3665 },
        { 0.754, 0.797, 0.825, 0.854, 0.884, 0.8990 }
    };
    double load = 0.0;
    if ( typeIndex >= 0 && typeIndex < 5 )
    {
        load = FBL_AspenInterpolate( curing, Load[typeIndex] );
    }
    return( load * 2000. / 43560. );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen dead 0.25 - 1.0" load.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen dead 0.25 - 1.0" load (lb/ft2).
 */

double FBL_AspenLoadDead10( int typeIndex, double /* curing */ )
{
    static double Load[] = { 0.975, 0.475, 1.035, 1.340, 1.115 };
    double load = 0.0;
    if ( typeIndex >= 0 && typeIndex < 5 )
    {
        load = Load[typeIndex];
    }
    return( load * 2000. / 43560. );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen live herbaceous load.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen live herbaceous load (lb/ft2).
 */

double FBL_AspenLoadLiveHerb( int typeIndex, double curing )
{
    static double Load[5][6] = {
        { 0.335, 0.234, 0.167, 0.100, 0.033, 0.000 },
        { 0.665, 0.465, 0.332, 0.199, 0.067, 0.000 },
        { 0.150, 0.105, 0.075, 0.045, 0.015, 0.000 },
        { 0.100, 0.070, 0.050, 0.030, 0.010, 0.000 },
        { 0.150, 0.105, 0.075, 0.045, 0.015, 0.000 }
    };
    double load = 0.0;
    if ( typeIndex >= 0 && typeIndex < 5 )
    {
        load = FBL_AspenInterpolate( curing, Load[typeIndex] );
    }
    return( load * 2000. / 43560. );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen live woody load.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen live woody load (lb/ft2).
 */

double FBL_AspenLoadLiveWoody( int typeIndex, double curing )
{
    static double Load[5][6] = {
        { 0.403, 0.403, 0.333, 0.283, 0.277, 0.274 },
        { 0.000, 0.000, 0.000, 0.000, 0.000, 0.000 },
        { 0.000, 0.000, 0.000, 0.000, 0.000, 0.000 },
        { 0.455, 0.455, 0.364, 0.290, 0.261, 0.2465 },
        { 0.000, 0.000, 0.000, 0.000, 0.000, 0.000 }
    };
    double load = 0.0;
    if ( typeIndex >= 0 && typeIndex < 5 )
    {
        load = FBL_AspenInterpolate( curing, Load[typeIndex] );
    }
    return( load * 2000. / 43560. );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen mortality rate.
 *
 *  \param severity Fire severity level: 0 = low severity, 1= moderate+ severity
 *  \param flameLength Flame length of the fire at the tree (ft).
 *  \param dbh          Aspen diameter at breast height (in).
 *
 *  \return Aspen mortality rate (fraction).
 */

double FBL_AspenMortality( int severity, double flameLength, double dbh )
{
    double mort = 1.;
    double ch = flameLength / 1.8;
    if ( severity == 0 )
    {
        mort = 1. / ( 1. + exp( -4.407 + 0.638 * dbh - 2.134 * ch ) );
    }
    else if ( severity == 1 )
    {
        mort = 1. / ( 1. + exp( -2.157 + 0.218 * dbh - 3.600 * ch ) );
    }
    mort = ( mort < 0.0 ) ? 0.0 : mort;
    mort = ( mort > 1.0 ) ? 1.0 : mort;
    return( mort );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen dead 0.0 - 0.25" savr.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen dead 0.0 - 0.25" savr (ft2/ft3).
 */

double FBL_AspenSavrDead1( int typeIndex, double curing )
{
    static double Savr[5][6] = {
        { 1440., 1620., 1910., 2090., 2220., 2285. },
        { 1480., 1890., 2050., 2160., 2240., 2280. },
        { 1400., 1540., 1620., 1690., 1750., 1780. },
        { 1350., 1420., 1710., 1910., 2060., 2135. },
        { 1420., 1540., 1610., 1670., 1720., 1745. }
    };
    double savr = 1440.;
    if ( typeIndex >= 0 && typeIndex < 5 )
    {
        savr = FBL_AspenInterpolate( curing, Savr[typeIndex] );
    }
    return( savr );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen dead 0.25 - 1.0" savr.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen dead 0.25 - 1.0" savr (ft2/ft3).
 */

double FBL_AspenSavrDead10( int /*typeIndex*/, double /*curing*/ )
{
    return( 109.0 );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen live herbaceous savr.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen live herbaceous savr (ft2/ft3).
 */

double FBL_AspenSavrLiveHerb( int /*typeIndex*/, double /*curing*/ )
{
    return( 2800.0 );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the aspen live woody savr.
 *
 *  \param typeIndex Index of the aspen fuel type:
 *                      0 = Aspen/shrub
 *                      1 = Aspen/tall forb
 *                      2 = Aspen/low forb
 *                      3 = Mixed/forb
 *                      4 = Mixed/shrub
 *  \param curing   Curing level (fraction)
 *
 *  \return Aspen live woody savr (ft2/ft3).
 */
double FBL_AspenSavrLiveWoody( int typeIndex, double curing )
{
    double Savr[5][6] = {
        { 2440., 2440., 2310., 2090., 1670., 1670. },
        { 2440., 2440., 2440., 2440., 2440., 2440. },
        { 2440., 2440., 2440., 2440., 2440., 2440. },
        { 2530., 2530., 2410., 2210., 1800., 1800. },
        { 2440., 2440., 2440., 2440., 2440., 2440. }
    };
    double savr = 2440.;
    if ( typeIndex >= 0 && typeIndex < 5 )
    {
        savr = FBL_AspenInterpolate( curing, Savr[typeIndex] );
    }
    return( savr );
}

//------------------------------------------------------------------------------
/*! \brief Constrains compass \a degrees to the range [0-360].
 *
 *  \param degrees Compass degrees (clockwise from north).
 *
 *  \return Compass degrees.
 */
double FBL_CompassConstrainDegrees( double degrees )
{
    while ( degrees > 360 )
    {
        degrees -= 360;
    }
    return( degrees );
}

//------------------------------------------------------------------------------
/*! \brief Converts compass degrees to radians.
 *
 *  \param degrees Compass degrees (clockwise from north).
 *
 *  \return Compass degrees.
 */
double FBL_CompassDegreesToRadians( double degrees )
{
    return( degrees * M_PI / 180. );
}

//------------------------------------------------------------------------------
/*! \brief Converts compass radians to degrees.
 *
 *  \param radians Compass radians (clockwise from north).
 *
 *  \return Compass radians.
 */
double FBL_CompassRadiansToDegrees( double radians )
{
    return( radians * 180. / M_PI );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire active ratio.
 *
 *  \param crownSpreadRate      Crown fire spread rate (ft/min).
 *  \param criticalSpreadRate   Critical crown fire spread rate (ft/min).
 *
 *  \return Crown fire active ratio.
 */

double FBL_CrownFireActiveRatio( double crownSpreadRate,
        double criticalSpreadRate )
{
    return( ( criticalSpreadRate < SMIDGEN )
          ? ( 0.00 )
          : ( crownSpreadRate / criticalSpreadRate ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire area from its forward spread distance and
 *  elliptical length-to-width ratio using the assumptions and equations as per
 *  Rothermel (1991) equation 11 on page 16 (which ignores backing distance).
 *
 *  \param spreadDistance Fire forward spread distance (ft).
 *  \param lwRatio Crown fire length-to-width ratio (ft/ft).
 *
 *  \return Fire area (ft2).
 */

double FBL_CrownFireArea( double spreadDistance, double lwRatio )
{
    return( M_PI * spreadDistance * spreadDistance / ( 4. * lwRatio ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the critical crown fire spread rate to achieve active
 *  crowning.
 *
 *  \param canopyBulkDensity    Canopy crown bulk density (lb/ft3).
 *
 *  \return Critical crown fire spread rate (ft/min).
 */

double FBL_CrownFireCriticalCrownFireSpreadRate( double canopyBulkDensity )
{
    double cbd = 16.0185 * canopyBulkDensity;       // Convert to Kg/m3
    double ros = ( cbd < SMIDGEN ) ? 0.00 : ( 3.0 / cbd );
    return( 3.28084 * ros );                        // Convert to ft/min
}

//------------------------------------------------------------------------------
/*! \brief SCott and Reinhardt equation for the critical surface ROS
 *  (R'initiation) used to determine crown fire initiation.
 *
 *  \param criticalSurfaceFireIntensity (Btu/fts)
 *	\param surfaceFireHpua
 *
 *  \return Critical surface fire spread rate (ft/min).
 */

double FBL_CrownFireCriticalSurfaceFireSpreadRate(
	double criticalSurfaceFireIntensity,
	double surfaceFireHpua )
{
	double ros = 99999.;
	if ( surfaceFireHpua > 0. )
	{
		ros = (60. * criticalSurfaceFireIntensity) / surfaceFireHpua;
	}
	return ros;
}

//------------------------------------------------------------------------------
/*! \brief Reverse calculates the critical surface fire intensity for a
 *  surface fire to transition to a crown fire given the critical flame length.
 *
 *  \param criticalFlameLength Critical surface fire flame length (ft).
 *
 *  \return Critical surface fire intensity (Btu/ft/s).
 */

double FBL_CrownFireCriticalSurfaceFireIntensity( double criticalFlameLength )
{
    return( FBL_SurfaceFireFirelineIntensity( criticalFlameLength ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the critical surface fire intensity for a surface fire
 *  to transition to a crown fire.
 *
 *  \param foliarMoisture   Tree foliar moisture content (lb water/lb foliage).
 *  \param crownBaseHt      Tree crown base height (ft).
 *
 *  \return Critical surface fire intensity (Btu/ft/s).
 */

double FBL_CrownFireCriticalSurfaceFireIntensity( double foliarMoisture,
        double crownBaseHt )
{
    // Convert foliar moisture content to percent and constrain lower limit
    double fmc = 100. * foliarMoisture;
    fmc = ( fmc < 30.0 ) ? 30. : fmc;
    // Convert crown base ht to meters and constrain lower limit
    double cbh = 0.3048 * crownBaseHt;
    cbh = ( cbh < 0.1 ) ? 0.1 : cbh;
    // Critical surface fireline intensity (kW/m)
    double csfi =pow( (0.010 * cbh * ( 460. + 25.9 * fmc ) ), 1.5 );
    // Return as Btu/ft/s
    return ( 0.288672 * csfi );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the critical surface fire flame length for a surface fire
 *  to transition to a crown fire given the critical fireline intensity.
 *
 *  \param criticalFireInt Critical surface fireline intensity (Btu/ft/s).
 *
 *  \return Critical surface fire flame length (ft).
 */

double FBL_CrownFireCriticalSurfaceFlameLength( double criticalFireInt )
{
    return( FBL_SurfaceFireFlameLength( criticalFireInt ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire fireline intensity
 *  given the surface fire and crown fire heats per unit area
 *  and the crown fire spread rate.
 *
 *  \param crownFireHpua Crown fire (surface + canopy) heat per unit area (Btu/ft2).
 *  \param crownFireSpreadRate Crown fire rate of spread (ft/min).
 *
 *  \return Crown fire fireline intensity (Btu/ft/s).
 */

double FBL_CrownFireFirelineIntensity( double crownFireHpua, double crownFireSpreadRate )
{
    return( (crownFireSpreadRate / 60.) * crownFireHpua );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fireline intensity given the fire flame length 
 *
 *	Based on Thomas (1963), which differs from Byram (1959).
 *
 *  \param Crown fire flame length (ft).
 *
 *  \return crownFirelineIntensity Crown fireline intensity (Btu/ft/s).
 */

double FBL_CrownFireFirelineIntensityFromFlameLength( double crownFireFlameLength )
{
    return( pow( ( 5. * crownFireFlameLength ), 1.5 ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire flame length 
 *  given the crown fireline intensity.
 *
 *	Based on Thomas (1963), which differs from Byram (1959).
 *
 *  \param crownFirelineIntensity Crown fireline intensity (Btu/ft/s).
 *
 *  \return Crown fire flame length (ft).
 */

double FBL_CrownFireFlameLength( double crownFirelineIntensity )
{
    return( 0.2 * pow( crownFirelineIntensity, (2./3.) ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the total crown fire heat per unit area
 *  given the surface HPUA and canopy HPUA contributions.
 *
 *  \param surfaceHpua Surface fire heat per unit area (Btu/ft2)
 *  \param canopyHpua Crwon canopy heat per unit area (Btu/ft2)
 *
 *  \return Total crown fire (surface plus canopy) heat per unit area (Btu/ft2).
 */

double FBL_CrownFireHeatPerUnitArea( double surfaceHpua,
    double canopyHpua )
{
    return( surfaceHpua + canopyHpua );
}
//------------------------------------------------------------------------------
/*! \brief Calculates the canopy portion of the crown fire heat per unit area
 *  given the crown fire fuel load and low heat of combustion.
 *
 *  \param crownFireFuelLoad Crown fire fuel load (lb/ft2).
 *  \param lowHeatOfCombustion Low heat of combustion (Btu/lb)
 *
 *  \return Crown fire canopy heat per unit area (Btu/ft2).
 */

double FBL_CrownFireHeatPerUnitAreaCanopy( double crownFuelLoad,
    double lowHeatOfCombustion )
{
    return( crownFuelLoad * lowHeatOfCombustion );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire length-to-width ratio given the 20-ft
 *  wind speed (Rothermel 1991, Equation 10, p16).
 *
 *  \param windSpeedAt20ft Wind speed at 20-ft (mi/h).
 *
 *  \return Crown fire length-to-width ratio (ft/ft).
 */

double FBL_CrownFireLengthToWidthRatio( double windSpeedAt20ft )
{
    return( ( windSpeedAt20ft > SMIDGEN )
          ? ( 1. + 0.125 * windSpeedAt20ft )
          : ( 1. ) );
}

//------------------------------------------------------------------------------
/*! \brief Estimates crown fire perimeter from spread distance and length-to-
 *  wifth ratio as per Rothermel (1991) equation 13 on page 16.
 *
 *  \param fireLength   Fire ellipse length (ft).
 *  \param fireWidth    Fire ellipse width (ft).
 *
 *  \return Fire perimeter (ft).
 */

double FBL_CrownFirePerimeter( double spreadDistance, double lwRatio )
{
    return( 0.5 * M_PI * spreadDistance * (1. + 1. / lwRatio ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire 'power of the fire'
 *  given the crown fireline intensity.
 *
 *  \param crownFirelineIntensity Crown fireline intensity (Btu/ft/s).
 *
 *  \return Crown fire 'power of the fire' (ft-lb/s/ft2).
 */

double FBL_CrownFirePowerOfFire( double crownFirelineIntensity )
{
    return( crownFirelineIntensity / 129. );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire 'power of the wind'
 *  given the 20-ft wind speed and crown fire spread rate.
 *
 *  \param windSpeedAt20Ft Wind speed at 20ft (ft/min).
 *  \param crownFireSPreadRate Crown fire spread rate (ft/min).
 *
 *  \return Crown fire 'power of the wind' (ft-lb/s/ft2).
 */

double FBL_CrownFirePowerOfWind( double windSpeedAt20Ft,
    double crownFireSpreadRate )
{
    double diff = ( windSpeedAt20Ft - crownFireSpreadRate ) / 60.;
    diff = ( diff < SMIDGEN ) ? SMIDGEN : diff;
    return( 0.00106 * diff * diff * diff );
}


//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire 'power-of-fire to power-of-wind' ratio.
 *
 *  \param firePower Power of the fire (ft-lb/s/ft2).
 *  \param windPower Power of the wind (ft-lb/s/ft2).
 *
 *  \return Ratio of the crown fire 'power-of-the-fire' to 'power-of-the-wind).
 */

double FBL_CrownFirePowerRatio( double firePower, double windPower )
{
    return( ( windPower > SMIDGEN ) ? ( firePower / windPower ) : 0.0 );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire spread rate.
 *
 *  This uses Rothermel's 1991 crown fire correlation.
 *
 *  \param windAt20Ft   Wind speed at 20 ft above the canopy (mi/h).
 *  \param mc1          Dead 1-h fuel moisture content (lb water/lb fuel).
 *  \param mc10         Dead 10-h fuel moisture content (lb water/lb fuel).
 *  \param mc100        Dead 100-h fuel moisture content (lb water/lb fuel).
 *  \param mcWood       Live wood fuel moisture content (lb water/lb fuel).
 *
 *  \return Crown fire average spread rate (ft/min).
 */
double FBL_CrownFireSpreadRate( double windAt20Ft, double mc1, double mc10,
        double mc100, double mcWood )
{
	double aspect              = 180.;
    double slopeFraction       = 0.0;
    double windDirFromUpslope  = 0.0;
    double midflameWindSpeed   = 0.4 * windAt20Ft;
	bool   applyWindSpeedLimit = true;
    double mois[4];
    mois[0] = mc1;
    mois[1] = mc10;
    mois[2] = mc100;
    mois[3] = mcWood;

#ifdef INCLUDE_OLD_CROWN_REFACTOR
	Bp6CrownFuelBedIntermediates fb;
	double beta = fb.getPackingRatio();
	double sigma = fb.getSigma();
	double propFlux = fb.getPropagatingFlux();

	Bp6SurfaceFuelHeatSink hs = Bp6SurfaceFuelHeatSink( &fb, mois );
	double heatSink = hs.getHeatSink();

	Bp6SurfaceFireReactionIntensity rx = Bp6SurfaceFireReactionIntensity( &hs );
	double rxInt = rx.getTotalRxInt();

	double ros0 = FBL_SurfaceFireNoWindNoSlopeSpreadRate( rxInt,
		propFlux, heatSink ) ;

	Bp6SurfaceFireForwardSpreadRate sr = Bp6SurfaceFireForwardSpreadRate(
		&fb, ros0, rxInt, slopeFraction, midflameWindSpeed, windDirFromUpslope );
	double rosMax = sr.getMaxSpreadRate();
    double crownRosOld = 3.34 * rosMax;
#endif

	// V6 Refactor
	Bp6CrownFire cf;
	cf.setMoisture( mois );
	cf.setSite(
		slopeFraction,
		aspect,
		88.*midflameWindSpeed,
		windDirFromUpslope,
		applyWindSpeedLimit );
	double crownRos = cf.getActiveCrownFireRos();

#ifdef INCLUDE_OLD_CROWN_REFACTOR
	if ( fabs( crownRos-crownRosOld ) > 1.0e-7 )
	{
		printf( "\n*** crown fire ros v6=%g, old=%g\n", crownRos, crownRosOld );
	}
#endif
    return( crownRos );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire transition ratio.
 *
 *  \param surfaceFireInt   Surface fireline intensity (Btu/ft/s).
 *  \param criticalFireInt  Critical crown fire fireline intensity (Btu/ft/s).
 *
 *  \return Transition ratio.
 */

double FBL_CrownFireTransitionRatio( double surfaceFireInt,
        double criticalFireInt )
{
    return( ( criticalFireInt < SMIDGEN )
          ? ( 0.00 )
          : ( surfaceFireInt / criticalFireInt ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates crown fire maximum width from its length and
 *  length-to-width ratio.
 *
 *  \param fireLength       Fire length (ft).
 *  \param lengthWidthRatio Fire length-to-width ratio (ft/ft).
 *
 *  \return Maximum fire width (ft).
 */

double FBL_CrownFireWidth( double fireLength, double lengthWidthRatio )
{
    return( ( lengthWidthRatio < SMIDGEN )
          ? ( 0.0 )
          : ( fireLength / lengthWidthRatio ) );
}
//------------------------------------------------------------------------------
/*! \brief Calculates the crown fire fuel load
 *  given the canopy bulk density and canopy height.
 *
 *  \param canopyBulkDensity Canopy bulk density (lb/ft3).
 *  \param canopyHt Canopy height (ft)
 *	\param baseHt Canopy base height (ft)
 *
 *  \return Crown fire fuel load (lb/ft2).
 */

double FBL_CrownFuelLoad( double canopyBulkDensity, double canopyHt, double baseHt )
{
    return( canopyBulkDensity * ( canopyHt - baseHt ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the dew point temperature.
 *
 *  \param dryBulb  Dry bulb air temperature (oF).
 *  \param wetBulb  Wet bulb air temperature (oF).
 *  \param elev     Elevation above mean sea level (ft).
 *
 *  \return         Dew point temperature (oF).
 */

double FBL_DewPointTemperature( double dryBulb, double wetBulb, double elev )
{
    double dbulbc = ( dryBulb - 32. ) * 5. / 9.;
    double wbulbc = ( wetBulb - 32. ) * 5. / 9.;
    double dewpoint = dryBulb;
    if ( wbulbc < dbulbc )
    {
        // double e1 = 6.1121 * exp( 17.502 * dbulbc / (240.97 + dbulbc) );
        double e2 = 6.1121 * exp( 17.502 * wbulbc / (240.97 + wbulbc) );
        if ( wbulbc < 0. )
        {
            e2 = 6.1115 * exp( 22.452 * wbulbc / ( 272.55 + wbulbc) );
        }
        double p = 1013. * exp( -0.0000375 * elev );
        double d = 0.66 * ( 1. + 0.00115 * wbulbc) * (dbulbc - wbulbc);
        double e3 = e2 - d * p / 1000.;
        if ( e3 < 0.001 )
        {
            e3 = 0.001;
        }
        double t3 = -240.97 /  ( 1.- 17.502 / log(e3 / 6.1121) );
        if ( ( dewpoint = t3 * 9. / 5. + 32. ) < -40. )
        {
            dewpoint = -40.;
        }
    }
    return( dewpoint );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire type; surface, passive, or active.
 *
 *  \param transitionRatio  Crown fire transition ratio.
 *  \param activeRatio      Crown fire active ratio.
 *
 *  \retval 0 == Surface: surface fire with no torching or crown fire spread.
 *  \retval 1 == Torching: surface fire with torching.
 *  \retval 2 == Conditional crown fire: conditions indicate the fire will
 *                  not transition from the surface to the crown.
 *                  But if it does, an active crown fire may result.
 *  \retval 3 == Crowing: active crown fire, fire is spreading through the canopy.
 */

int FBL_FireType( double transitionRatio, double activeRatio )
{
    int status = 0;
    // If the fire CAN NOT transition to the crown ...
    if ( transitionRatio < 1.0 )
    {
        if ( activeRatio < 1.0 )
        {
            status = 0;     // Surface fire
        }
        else // if ( activeRatio >= 1.0 )
        {
            status = 2;     // Conditional crown fire
        }
    }
    // If the fire CAN transition to the crown ...
    else // if ( transitionRation >= 1.0 )
    {
        if ( activeRatio < 1.0 )
        {
            status = 1;     // Torching
        }
        else // if ( activeRatio >= 1.0 )
        {
            status = 3;     // Crowning
        }
    }
    return( status );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the heat index using the algorithm from
 *  http://www.usatoday.com/weather/whumcalc.htm and
 *  http://www.srh.noaa.gov/elp/wxcalc/heatindexsc.html
 *
 *  \param at Air temperature (oF).
 *  \param rh Air relative humidity (%).
 *
 *  \return Heat index.
 */

double FBL_HeatIndex1( double at, double rh )
{
    return( -42.379
        + 2.04901523 * at
        + 10.14333127 * rh
        - 0.22475541 * at * rh
        - 6.83783e-03 * at * at
        - 5.481717e-02 * rh * rh
        + 1.22874e-03 * at * at * rh
        + 8.5282e-04 * at * rh * rh
        - 1.99e-06 * at * at * rh * rh );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the heat index using the algorithm from
 *  http://www.wvec.com/knowledge/heatindex.htm
 *
 *  \param at Air temperature (oF).
 *  \param rh Air relative humidity (%).
 *
 *  \return Heat index.
 */

double FBL_HeatIndex2( double at, double rh )
{
    return( 16.923
        + 0.185212e+00 * at
        + 0.537941e+01 * rh
        - 0.100254e+00 * at * rh
        + 0.941695e-02 * at * at
        + 0.728898e-02 * rh * rh
        + 0.345372e-03 * at * at * rh
        - 0.814970e-03 * at * rh * rh
        + 0.102102e-04 * at * at * rh * rh
        - 0.386460e-04 * at * at * at
        + 0.291583e-04 * rh * rh * rh
        + 0.142721e-05 * at * at * at * rh
        + 0.197483e-06 * at * rh * rh * rh
        - 0.218429e-07 * at * at * at * rh * rh
        + 0.843296e-09 * at * at * rh * rh * rh
        - 0.481975e-10 * at * at * at * rh * rh * rh );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fraction of live herbaceous fuel that is cured.
 *
 *  \param moistureContent Live herbaceous fuel moisture content (fraction).
 *
 *  \return Fraction [0..1] of live herbaceous fuel that is cured.
 */

double FBL_HerbaceousFuelLoadCuredFraction( double moistureContent )
{
    double fraction = 1.333 - 1.11 * moistureContent;
    fraction = ( fraction < 0.0 ) ? 0.0 : fraction;
    fraction = ( fraction > 1.0 ) ? 1.0 : fraction;
    return( fraction );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the palmetto-gallberry dead 0.0 - 0.25" load.
 *
 *  \param age      Age of rough (years).
 *  \param height   Height of the understory (ft).
 *
 *  \return Palmetto-gallbery dead 0.0 - 0.25" load (lb/ft2).
 */

double FBL_PalmettoGallberyDead1HrLoad( double age, double height )
{
    double load = -0.00121
                + 0.00379 * log( age )
                + 0.00118 * height * height;
    if ( load < 0.0 )
    {
        load = 0.0;
    }
    return( load );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the palmetto-gallberry dead 0.25 - 1.0 " load.
 *
 *  \param age      Age of rough (years).
 *  \param cover    Coverage of area by palmetto (percent).
 *
 *  \return Palmetto-gallbery dead 0.25 - 1.0" load (lb/ft2).
 */

double FBL_PalmettoGallberyDead10HrLoad( double age, double cover )
{
    double load = -0.00775
                + 0.00021 * cover
                + 0.00007 * age * age;
    if ( load < 0.0 )
    {
        load = 0.0;
    }
    return( load );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the palmetto-gallberry dead foliage load.
 *
 *  \param age      Age of rough (years).
 *  \param cover    Coverage of area by palmetto (percent).
 *
 *  \return Palmetto-gallbery dead foliage load (lb/ft2).
 */

double FBL_PalmettoGallberyDeadFoliageLoad( double age, double cover )
{
    return( 0.00221 * pow( age, 0.51263  ) * exp( 0.02482 * cover ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the palmetto-gallberry fuel bed depth.
 *
 *  \param height   Height of the understory (ft).
 *
 *  \return Palmetto-gallbery fuel bed depth (ft).
 */

double FBL_PalmettoGallberyFuelBedDepth( double height )
{
    return( 2. * height / 3. );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the palmetto-gallberry L layer load.
 *
 *  \param age      Age of rough (years).
 *  \param ba       Overstory basal aea (ft2/ac)
 *
 *  \return Palmetto-gallbery L layer load (lb/ft2).
 */

double FBL_PalmettoGallberyLitterLoad( double age, double ba )
{
    return( ( 0.03632 + 0.0005336 * ba ) * ( 1.0 - pow( 0.25, age ) ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the palmetto-gallberry live 0.0 - 0.25" load.
 *
 *  \param age      Age of rough (years).
 *  \param height   Height of the understory (ft).
 *
 *  \return Palmetto-gallbery live 0.0 - 0.25" load (lb/ft2).
 */

double FBL_PalmettoGallberyLive1HrLoad( double age, double height )
{
    return( 0.00546 + 0.00092 * age + 0.00212 * height * height );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the palmetto-gallberry live 0.25 - 1.0" load.
 *
 *  \param age      Age of rough (years).
 *  \param height   Height of the understory (ft).
 *
 *  \return Palmetto-gallbery live 0.25 - 1.0" load (lb/ft2).
 */

double FBL_PalmettoGallberyLive10HrLoad( double age, double height )
{
    double load = -0.02128
                + 0.00014 * age * age
                + 0.00314 * height * height;
    if ( load < 0.0 )
    {
        load = 0.0;
    }
    return( load );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the palmetto-gallberry live foliage load.
 *
 *  \param age      Age of rough (years).
 *  \param cover    Coverage of area by palmetto (percent).
 *  \param height   Height of the understory (ft).
 *
 *  \return Palmetto-gallbery live foliage load (lb/ft2).
 */

double FBL_PalmettoGallberyLiveFoliageLoad( double age, double cover,
        double height )
{
    double load = -0.0036
                + 0.00253 * age
                + 0.00049 * cover
                + 0.00282 * height * height;
    if ( load < 0.0 )
    {
        load = 0.0;
    }
    return( load );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the length of a SQUARE fire safety zone for personnel and equipment.
 *
 *  This is roughly the distance at which the heat flux drops to 7 kW/m2.
 *
 *  \param sepDist  Separation distance (ft)
 *  \param pNumb    Number of people to be sheltered in the safety zone.
 *  \param pArea    Mean area required per person within the safety zone (ft2).
 *  \param eNumb    Number of pieces of heavy equipment
 *                  to be sheltered in the safety zone.
 *  \param eArea    Mean area occupied by each piece of heavy equipment (ft2).
 *
 *  \return Length of a square, cleared safety zone within which people and
 *          equipment are exposed to less than 7 kW/m2 heat flux.
 */

double FBL_SafetyZoneLength( double sepDist, double pNumb, double pArea,
        double eqNumb, double eqArea )
{
	double radius = FBL_SafetyZoneRadius( sepDist, pNumb, pArea, eqNumb, eqArea );
	double length = 2.0 * radius;
	return( length );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the radius of a CIRCULAR fire safety zone for personnel and equipment.
 *
 *  This is roughly the distance at which the heat flux drops to 7 kW/m2.
 *
 *  \param sepDist  Separation distance (ft)
 *  \param pNumb    Number of people to be sheltered in the safety zone.
 *  \param pArea    Mean area required per person within the safety zone (ft2).
 *  \param eNumb    Number of pieces of heavy equipment
 *                  to be sheltered in the safety zone.
 *  \param eArea    Mean area occupied by each piece of heavy equipment (ft2).
 *
 *  \return Radius of a cleared safety zone within which people and
 *          equipment are exposed to less than 7 kW/m2 heat flux.
 */

double FBL_SafetyZoneRadius( double sepDist, double pNumb, double pArea,
        double eqNumb, double eqArea )
{
    // Space needed by firefighters and equipment in core of safety zone
    double coreRadius = ( pArea * pNumb + eqNumb * eqArea ) / M_PI;
    if ( coreRadius > SMIDGEN )
    {
        coreRadius = sqrt( coreRadius );
    }
    // Add 4 times the flame ht to the protected safety zone core
    double fullRadius = sepDist + coreRadius;
    return( fullRadius );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire safety zone separation distance for personnel
 *      and equipment.
 *
 *      This is roughly the distance at which the heat flux drops to 7 kW/m2.
 *
 *  \param flameHt  Height of the flame front (ft).
 *
 *  \return Separation distance between the flame front and personnel/equipment
 */

double FBL_SafetyZoneSeparationDistance( double flameHt )
{
    return( 4. * flameHt );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the relative humidity.
 *
 *  \param dryBulb  Air temperature (oF).
 *  \param dewPt    Dew point temperature (oF).
 *
 *  \return Relative humidity (fraction).
 */

double FBL_RelativeHumidity( double dryBulb, double dewPt )
{
    return( ( dewPt >= dryBulb )
          ? ( 1.0 )
          : ( exp( -7469. / ( dewPt+398.0 ) + 7469. / ( dryBulb+398.0 ) ) ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates cover height used in spotting distance calculations.
 *
 *  \param z        Maximum firebrand height.
 *  \param coverHt  Tree/vegetation cover height (ft).
 *
 *  \return Cover ht used in calculation of flat terrain spotting distance.
 */

double FBL_SpotCriticalCoverHt( double z, double coverHt )
{
    // Minimum value of coverHt used to calculate flatDist
    // using log variation with ht.
    double criticalHt = ( z < SMIDGEN )
                      ? ( 0.0 )
                      : ( 2.2 * pow( z, 0.337 ) - 4.0 );

    // Cover ht used in calculation of flatDist.
    double htUsed = ( coverHt > criticalHt )
                  ? ( coverHt )
                  : ( criticalHt ) ;
    return( htUsed );
}

//------------------------------------------------------------------------------
/*! \brief Calculates maximum spotting distance over flat terrain.
 *
 *  \param firebrandHt      Maximum firebrand height (ft).
 *  \param coverHt          Tree/vegetation cover height (ft).
 *  \param windSpeedAt20Ft  Wind speed at 20 ft (mi/h).
 *
 *  \return Maximum spotting distance over flat terrain.
 */

double FBL_SpotDistanceFlatTerrain(
            double firebrandHt,
            double coverHt,
            double windSpeedAt20Ft )
{
    // Flat terrain spotting distance.
    double distance = 0.;
    if ( coverHt > SMIDGEN )
    {
        distance = 0.000718 * windSpeedAt20Ft * sqrt( coverHt )
                 * (0.362 + sqrt( firebrandHt / coverHt ) / 2.
                 * log( firebrandHt / coverHt ) );
    }
    return( distance );
}

//------------------------------------------------------------------------------
/*! \brief Calculates maximum spotting distance from a burning pile.
 *
 *  \param location          Location of the burning pile:
 *                              0 == midslope, windward
 *                              1 == valley bottom
 *                              2 == midslope, leeward
 *                              3 == ridge top
 *  \param ridgeToValleyDist Horizontal distance from ridge top to valley bottom
 *                           (mi).
 *  \param ridgeToValleyElev Vertical distance from ridge top to valley bottom
 *                           (ft).
 *  \param coverHt           Tree/vegetation cover height (ft).
 *	\param openCanopy		 1 if downwind canopy is open, 0 if downwind canopy is closed
 *  \param windSpeedAt20Ft   Wind speed at 20 ft (mi/h).
 *  \param flameHt           Burning pile's flame height (ft).
 *  \param[out] htUsed       Actual tree/vegetation ht used (ft).
 *  \param[out] firebrandHt  Initial maximum firebrand height (ft).
 *  \param[out] flatDistance Maximum spotting distance over flat terrain (mi).
 *
 *  \return Maximum spotting distance from the burning pile (mi).
 */

double FBL_SpotDistanceFromBurningPile(
            int    location,
            double ridgeToValleyDist,
            double ridgeToValleyElev,
            double coverHt,
			int    openCanopy,
            double windSpeedAt20Ft,
            double flameHt,
            double *htUsed,
            double *firebrandHt,
            double *flatDistance )
{
    // Initialize return values
    double z        = 0.;
    double ht       = 0.;
    double flatDist = 0.;
    double mtnDist  = 0.;

    // Determine maximum firebrand height
    if ( windSpeedAt20Ft > SMIDGEN
      && flameHt > SMIDGEN )
    {
        // Determine maximum firebrand height
        z = 12.2 * flameHt;

		// Adjust downwind canopy height based upon canopy cover
		// Added in Release6 by Issues #028FAH - Downwind Canopy Open/Closed
		coverHt = ( openCanopy ) ? 0.5 * coverHt : coverHt;

        // Cover ht used in calculation of flatDist.
        if ( ( ht = FBL_SpotCriticalCoverHt( z, coverHt ) ) > SMIDGEN )
        {
            // Flat terrain spotting distance.
            flatDist = FBL_SpotDistanceFlatTerrain( z, ht, windSpeedAt20Ft );
            // Adjust for mountainous terrain.
            mtnDist = FBL_SpotDistanceMountainTerrain( flatDist,
                location, ridgeToValleyDist, ridgeToValleyElev );
        } // if ht > SMIDGEN
    } // if windSpeedAt20Ft > SMIDGEN && z > SMIDGEN

    // Store return values and return max spot distance over mountainous terrain
    if ( htUsed )       *htUsed       = ht;
    if ( firebrandHt )  *firebrandHt  = z;
    if ( flatDistance ) *flatDistance = flatDist;
    return( mtnDist );
}

//------------------------------------------------------------------------------
/*! \brief Calculates maximum spotting distance from a surface fire.
 *
 *  \param location          Location of the burning pile:
 *                              0 == midslope, windward
 *                              1 == valley bottom
 *                              2 == midslope, leeward
 *                              3 == ridge top
 *  \param ridgeToValleyDist Horizontal distance from ridge top to valley bottom
 *                           (mi).
 *  \param ridgeToValleyElev Vertical distance from ridge top to valley bottom
 *                           (ft).
 *  \param coverHt           Downwind tree/vegetation cover height (ft).
 *	\param openCanopy		 1 if downwind canopy is open, 0 if downwind canopy is closed
 *  \param windSpeedAt20Ft   Wind speed at 20 ft (mi/h).
 *  \param flameLength       Surface fire flame length (ft).
 *  \param[out] htUsed       Actual tree/vegetation ht used (ft).
 *  \param[out] firebrandHt  Initial maximum firebrand height (ft).
 *  \param[out] firebrandDrift Maximum firebrand drift (mi).
 *  \param[out] flatDistance Maximum spotting distance over flat terrain (mi).
 *
 *  \return Maximum sptting distance from the surface fire (mi).
 */

double FBL_SpotDistanceFromSurfaceFire(
            int    location,
            double ridgeToValleyDist,
            double ridgeToValleyElev,
            double coverHt,
			int    openCanopy,
            double windSpeedAt20Ft,
            double flameLength,
            double *htUsed,
            double *firebrandHt,
            double *firebrandDrift,
            double *flatDistance )
{
    // Initialize return variables
    double z        = 0.;
    double ht       = 0.;
    double flatDist = 0.;
    double mtnDist  = 0.;
    double drift    = 0.;

    // Determine maximum firebrand height
    if ( windSpeedAt20Ft > SMIDGEN
      && flameLength > SMIDGEN )
    {
        // f is function relating thermal energy to windspeed.
        double f = 322. * pow( ( 0.474 * windSpeedAt20Ft ), -1.01 );

        // Byram's fireline intensity is derived back from flame length.
        double byrams = pow( ( flameLength / .45 ), ( 1. / 0.46 ) );

        // Initial firebrand height (ft).
        z = ( (f * byrams) < SMIDGEN )
             ? ( 0.0 )
             : ( 1.055 * sqrt( f * byrams ) );

		// Adjust downwind canopy height based upon canopy cover
		// Added in Release6 by Issues #028FAH - Downwind Canopy Open/Closed
		coverHt = ( openCanopy ) ? 0.5 * coverHt : coverHt;

        // Cover ht used in calculation of flatDist.
        if ( ( ht = FBL_SpotCriticalCoverHt( z, coverHt ) ) > SMIDGEN )
        {
            drift    = 0.000278 * windSpeedAt20Ft * pow( z, 0.643 );
            flatDist = FBL_SpotDistanceFlatTerrain( z, ht, windSpeedAt20Ft )
                     + drift;
            mtnDist  = FBL_SpotDistanceMountainTerrain( flatDist,
                            location, ridgeToValleyDist, ridgeToValleyElev );
        } // if ht > SMIDGEN
    } // if windSpeedAt20Ft > SMIDGEN && flameLength > SMIDGEN

    // Store return values and return max spot distance over mountainous terrain
    if ( htUsed )         *htUsed         = ht;
    if ( firebrandHt )    *firebrandHt    = z;
    if ( firebrandDrift ) *firebrandDrift = drift;
    if ( flatDistance )   *flatDistance   = flatDist;
    return( mtnDist );
}

//------------------------------------------------------------------------------
/*! \brief Calculates maximum spotting distance from a group of torching trees.
 *
 *  \param location          Location of the burning pile:
 *                              0 == midslope, windward
 *                              1 == valley bottom
 *                              2 == midslope, leeward
 *                              3 == ridge top
 *  \param ridgeToValleyDist Horizontal distance from ridge top to valley bottom
 *                           (mi).
 *  \param ridgeToValleyElev Vertical distance from ridge top to valley bottom
 *                           (ft).
 *  \param coverHt           Tree/vegetation cover height (ft).
 *	\param openCanopy		 1 if downwind canopy is open, 0 if downwind canopy is closed
 *  \param windSpeedAt20Ft   Wind speed at 20 ft (mi/h).
 *  \param torchingTrees     Number of torching trees.
 *  \param treeDbh           Tree dbh (in).
 *  \param treeHt            Tree height (ft).
 *  \param treeSpecies       Tree species code.
 *  \param[out] htUsed       Actual tree/vegetation ht used (ft).
 *  \param[out] flameHt      Steady state flame ht (ft).
 *  \param[out] flameRatio   Ratio of tree height to steady flame height (ft/ft).
 *  \param[out] flameDur     Flame duration (min).
 *  \param[out] firebrandHt  Initial maximum firebrand height (ft).
 *  \param[out] flatDistance Maximum spotting distance over flat terrain (mi).
 *
 *  \return Maximum spotting distance from the torching trees (mi).
 */

static double TorchA[][4] = {
    {15.7, .451, 12.6, -.256},  //  0 Engelmann spruce
    {15.7, .451, 10.7, -.278},  //  1 Douglas-fir
    {15.7, .451, 10.7, -.278},  //  2 subalpine fir
    {15.7, .451,  6.3, -.249},  //  3 western hemlock
    {12.9, .453, 12.6, -.256},  //  4 ponderosa pine
    {12.9, .453, 12.6, -.256},  //  5 lodgepole pine
    {12.9, .453, 10.7, -.278},  //  6 western white pine
    {16.5, .515, 10.7, -.278},  //  7 grand fir
    {16.5, .515, 10.7, -.278},  //  8 balsam fir
    {2.71, 1.00, 11.9, -.389},  //  9 slash pine
    {2.71, 1.00, 11.9, -.389},  // 10 longleaf pine
    {2.71, 1.00, 7.91, -.344},  // 11 pond pine
    {2.71, 1.00, 7.91, -.344},  // 12 shortleaf pine
    {2.71, 1.00, 13.5, -.544}   // 13 loblolly pine
//  {12.9, .453,  6.3, -.249},  // 14 western larch (guessed)
//  {15.7, .515, 12.6, -.256}   // 15 western red cedar (guessed)
} ;

static double TorchB[4][2] = {
    {4.24, 0.332},
    {3.64, 0.391},
    {2.78, 0.418},
    {4.70, 0.000}
};

double FBL_SpotDistanceFromTorchingTrees(
            int    location,
            double ridgeToValleyDist,
            double ridgeToValleyElev,
            double coverHt,
			int    openCanopy,
            double windSpeedAt20Ft,
            double torchingTrees,
            double treeDbh,
            double treeHt,
            int    treeSpecies,
            double *htUsed,
            double *flameHt,
            double *flameRatio,
            double *flameDur,
            double *firebrandHt,
            double *flatDistance )
{
    // Initialize potential return variables
    double ratio    = 0.;
    double stHt     = 0.;
    double dur      = 0.;
    double z        = 0.;
    double ht       = 0.;
    double flatDist = 0.;
    double mtnDist  = 0.;

    // Determine maximum firebrand height
    if ( windSpeedAt20Ft > SMIDGEN
      && treeDbh > SMIDGEN
      && torchingTrees >= 1. )
    {
        // Catch species errors.
        if ( treeSpecies < 0 || treeSpecies >= 14 )
        {
            return( mtnDist );
        }
        // Steady flame height (ft).
        stHt = TorchA[treeSpecies][0]
             * pow( treeDbh, TorchA[treeSpecies][1] )
             * pow( torchingTrees, 0.4 );
        ratio = treeHt / stHt;
        // Steady flame duration.
        dur  = TorchA[treeSpecies][2]
             * pow( treeDbh, TorchA[treeSpecies][3] )
             * pow( torchingTrees, -0.2 );
        int j;
        if ( ratio >= 1. )
        {
            j = 0;
        }
        else if ( ratio >= 0.5 )
        {
            j = 1;
        }
        else if ( dur < 3.5 )
        {
            j = 2;
        }
        else
        {
            j = 3;
        }
        // Initial firebrand height (ft).
        z = TorchB[j][0] * pow( dur, TorchB[j][1] ) * stHt + treeHt / 2.;

		// Adjust downwind canopy height based upon canopy cover
		// Added in Release6 by Issues #028FAH - Downwind Canopy Open/Closed
		coverHt = ( openCanopy ) ? 0.5 * coverHt : coverHt;

        // Cover ht used in calculation of flatDist.
        if ( ( ht = FBL_SpotCriticalCoverHt( z, coverHt ) ) > SMIDGEN )
        {
            flatDist = FBL_SpotDistanceFlatTerrain( z, ht, windSpeedAt20Ft );
            mtnDist  = FBL_SpotDistanceMountainTerrain( flatDist,
                            location, ridgeToValleyDist, ridgeToValleyElev );
        } // if ht > SMIDGEN
    } // if windSpeedAt20Ft > SMIDGEN && dbh > SMIDGEN && tnum >= 1.

    // Store return values and return max spot distance over mountainous terrain
    if ( htUsed )       *htUsed       = ht;
    if ( flameHt )      *flameHt      = stHt;
    if ( flameRatio )   *flameRatio   = ratio;
    if ( flameDur )     *flameDur     = dur;
    if ( firebrandHt )  *firebrandHt  = z;
    if ( flatDistance ) *flatDistance = flatDist;
    return( mtnDist );
}

//------------------------------------------------------------------------------
/*! \brief Calculates maximum spotting distance adjusted for mountain terrain.
 *
 *  \param flatDist          Maximum spotting distance over flat terrain (mi).
 *  \param location          Location of the burning pile:
 *                              0 == midslope, windward
 *                              1 == valley bottom
 *                              2 == midslope, leeward
 *                              3 == ridge top
 *  \param ridgeToValleyDist Horizontal distance from ridge top to valley bottom
 *                           (mi).
 *  \param ridgeToValleyElev Vertical distance from ridge top to valley bottom
 *                           (ft).
 *  \return Maximum spotting distance from the torching trees (mi).
 */

double FBL_SpotDistanceMountainTerrain(
            double flatDist,
            int    location,
            double ridgeToValleyDist,
            double ridgeToValleyElev )
{
    double mtnDist = flatDist;
    if ( ridgeToValleyElev > SMIDGEN
      && ridgeToValleyDist > SMIDGEN )
    {
        double a1 = flatDist / ridgeToValleyDist;
        double b1 = ridgeToValleyElev / ( 10. * M_PI ) / 1000.;
        double x = a1;
        for ( int i=0; i<6; i++ )
        {
            x = a1 - b1 * ( cos( M_PI * x - location * M_PI / 2. )
              - cos( location * M_PI / 2. ) );
        }
        mtnDist = x * ridgeToValleyDist;
    }
    return( mtnDist );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the summer simmer index using the algorithm from
 *  http://www.usatoday.com/weather/whumcalc.htm.
 *
 *  \param at Air temperature (oF).
 *  \param rh Relative humidity(%).
 *
 *  \return Summer simmer index (dl).
 */

double FBL_SummerSimmerIndex( double at, double rh )
{
    return( 1.98 * ( at - ( 0.55 - 0.0055*rh ) * ( at - 58. ) ) - 56.83 );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire area from its length and width assuming a
 *  simple elliptical shape.
 *
 *  \param fireLength Fire length (ft).
 *  \param fireWidth  Fire length (ft).
 *
 *  \return Fire area (ft2).
 */

double FBL_SurfaceFireArea( double fireLength, double fireWidth )
{
    return( M_PI * ( 0.5 * fireLength ) * ( 0.5 * fireWidth ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire's backing spread rate from its forward spread
 *  rate and elliptical eccentricity.
 *
 *  \param headSpreadRate Fire spread rate at the head (ft/min).
 *  \param eccentricity Fire eccentricity (fraction).
 *
 *  \return Fire's backing spread rate (ft/min).
 */

double FBL_SurfaceFireBackingSpreadRate( double headSpreadRate,
                double eccentricity )
{
    return( headSpreadRate * ( 1. - eccentricity ) / ( 1. + eccentricity ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates elliptical fire perimeter eccentricity from fire
 *  length-to-width ratio.
 *
 *  \param lengthWidthRatio Elliptical fire perimeter length-to-width ratio
 *  (ft/ft).
 *
 *  \return Elliptical fire eccentricity (dl).
 */

double FBL_SurfaceFireEccentricity( double lengthWidthRatio )
{
    double x = lengthWidthRatio * lengthWidthRatio - 1.0;
    return( ( x > 0. )
          ? ( sqrt( x ) / lengthWidthRatio )
          : ( 0.0 ) );
}

#ifdef INCLUDE_V5_CODE
//------------------------------------------------------------------------------
/*! \brief Performs a reverse calculation of the effective wind speed
 *  along some fire spread vector.
 *
 *  The reverse caluclation of effective wind speed is made from
 *      -# the fire spread rate along some vector and
 *      -# the no-wind, no-slope spread rate
 *  under the assumption of elliptical fire area growth.
 *
 *  \param noWindSpreadRate Fire no-wind, no-slope spread rate (ft/min)
 *  \param vectorSpreadRate Fire spread rate at head or any other vector (ft/min).
 *
 *  \par Note: a previous call must have been made to FBL_FuelBedIntermediates()
 *  to initialize \a m_windB and \a m_windE.
 *
 *  \return Effective wind speed (combined wind-slope effect) at the same
 *  vector as the \a vectorSpreadRate (mi/h).
 */
double FBL_SurfaceFireEffectiveWindSpeedAtVector( double noWindSpreadRate,
                double vectorSpreadRate )
{
    double phiEw   = ( noWindSpreadRate < SMIDGEN )
                   ? ( 0 )
                   : ( vectorSpreadRate / noWindSpreadRate - 1. );
    double effWind = ( ( phiEw * m_windE ) < SMIDGEN || m_windB < SMIDGEN )
                   ? ( 0. )
                   : ( pow( ( phiEw * m_windE ), ( 1. / m_windB ) ) );
    // Convert from ft/min t0 mi/h.
    return( effWind / 88. );
}
#endif

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse angle 'beta' in degrees given 'theta' in degrees.
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param g Ellipse parameter g (half the length of the major axis minus the
 *           backing distance). See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param thetaDegrees 'Theta', the angle in degrees between the direction of maximum spread
 *           (the ellipse's major axis) and a line segment between the fire ellipse
 *            center and the point 'p' determined by \a betaDegrees.
 *
 *  \return 'Beta', the angle (degrees) between the direction of maximum fire
 *           spread (the ellipse's major axis) and an arbitrary vector emanating
 *           from the \a ignition \a point.  The arbitrary vector intersects the
 *           fire ellipse perimeter at some point 'p'.
 */
double FBL_SurfaceFireEllipseBetaFromThetaDegrees( double f, double g, double h,
        double thetaDegrees )
{
    double thetaRadians = FBL_CompassDegreesToRadians(
                            FBL_CompassConstrainDegrees( thetaDegrees ) );
    double betaRadians = FBL_SurfaceFireEllipseBetaFromThetaRadians( f, g, h,
                            thetaRadians );
    double betaDegrees = FBL_CompassRadiansToDegrees( betaRadians );
    return( betaDegrees );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse angle 'beta' in radians given 'theta' in radians.
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param g Ellipse parameter g (half the length of the major axis minus the
 *           backing distance). See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param thetaRadians Theta, the angle in radians between the direction of maximum spread
 *           (the ellipse's major axis) and a line segment between the fire ellipse
 *           center and the point 'p' determined by \a betaRadians.
 *
 *  \param 'Beta', the angle (radians) between the direction of maximum fire
 *           spread (the ellipse's major axis) and an arbitrary vector emanating
 *           from the \a ignition \a point.  The arbitrary vector intersects the
 *           fire ellipse perimeter at some point 'p'.
 *
 */
double FBL_SurfaceFireEllipseBetaFromThetaRadians( double f, double g, double h,
        double thetaRadians )
{
	double num = h * sin( thetaRadians );
	double denom = g + f * cos( thetaRadians );
	double betaRadians = ( denom != 0. ) ? atan( num / denom ) : 0.;

	if ( betaRadians < 0. )
	{
		betaRadians += M_PI;
	}
	if ( thetaRadians > M_PI )
	{
		betaRadians += M_PI;
	}
    return( betaRadians );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse distance 'f' (see Catchpole et al, 1982),
 *  half the length of the ellipse's major axis.
 *
 *  \param fireLength Total fire ellipse length (any distance units).
 *
 *  \return Fire ellipse distance 'f' (half the ellipse's major axis distance),
 *  in the same units as for \a fireLength.
 */

double FBL_SurfaceFireEllipseF( double fireLength )
{
    return( 0.5 * fireLength );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse distance 'g' (see Catchpole et al, 1982),
 *  half the length of the ellipse's major axis less the backing distance.
 *
 *  \param fireLength Total fire ellipse length
 *  (any distance units, must be the same as those for \a backingDist).
 *  \param backingDist Backing distance from the ignition point to the rear
 *  (any distance units, must be the same as those for \a fireLength).
 *
 *  \return Fire ellipse distance 'g' (half the ellipse's major axis distance
 *  minus the backing distance), in the same units as for \a fireLength.
 */

double FBL_SurfaceFireEllipseG( double fireLength, double backingDist )
{
    return( ( 0.5 * fireLength ) - backingDist );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse distance 'h' (see Catchpole et al, 1982),
 *  half the length of the ellipse's minor axis.
 *
 *  \param fireWidth Total fire ellipse width (any distance units).
 *
 *  \return Fire ellipse distance 'h' (half the ellipse's minor axis distance),
 *  in the same units as for \a fireWidth.
 */

double FBL_SurfaceFireEllipseH( double fireWidth )
{
    return( 0.5 * fireWidth );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse angle 'psi' in degrees.
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param theta Angle in degrees between the direction of maximum fire spread
 *           (the ellipse's major axis) and an arbitrary vector emanating
 *           from the ellipse \a center.  The arbitrary vector intersects
 *           the fire ellipse perimeter at some point 'p'.
 *
 *  \return Psi, the angle in degrees between direction of maximum fire spread
 *  (the ellipse's major axis) and the normal of the tangent to the ellipse
 *  at point 'p' as determined by \a thetaDegrees.
 */

double FBL_SurfaceFireEllipsePsiFromThetaDegrees( double f, double h, double thetaDegrees )
{
    double thetaRadians = FBL_CompassDegreesToRadians(
                            FBL_CompassConstrainDegrees( thetaDegrees ) );
    double psiRadians   = FBL_SurfaceFireEllipsePsiFromThetaRadians( f, h, thetaRadians );
    double psiDegrees   = FBL_CompassRadiansToDegrees( psiRadians );
    return( psiDegrees );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse angle 'psi' in radians.
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param theta Angle in radians between the direction of maximum fire spread
 *           (the ellipse's major axis) and an arbitrary vector emanating
 *           from the ellipse \a center.  The arbitrary vector intersects
 *           the fire ellipse perimeter at some point 'p'.
 *
 *  \return Psi, the angle in radians between direction of maximum fire spread
 *  (the ellipse's major axis) and the normal of the tangent to the ellipse
 *  at point 'p' as determined by \a thetaRadians.
 */

double FBL_SurfaceFireEllipsePsiFromThetaRadians( double f, double h, double thetaRadians )
{
	if ( f == 0. || h == 0. )
	{
		return( 0. );
	}
    double tanPsi     = tan( thetaRadians ) * f / h;
    double psiRadians = atan( tanPsi );

	// Adjust for quadrant
	double boundary1 = 0.5 * M_PI;
	double boundary2 = 1.5 * M_PI;
	if ( thetaRadians < boundary1 )
	{
		// No adjustment needed
	}
	else if ( thetaRadians >= boundary1 && thetaRadians <= boundary2 )
	{
		psiRadians += M_PI;
	}
	else if ( thetaRadians > boundary2 )
	{
		psiRadians += (2. * M_PI);
	}
    return( psiRadians );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse angle 'theta' in degrees given beta in degrees.
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param g Ellipse parameter g (half the length of the major axis minus the
 *           backing distance). See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param betaDegrees Angle (degrees) between the direction of maximum fire
 *           spread (the ellipse's major axis) and an arbitrary vector emanating
 *           from the \a ignition \a point.  The arbitrary vector intersects the
 *           fire ellipse perimeter at some point 'p'.
 *
 *  \return Theta, the angle in degrees between the direction of maximum spread
 *  (the ellipse's major axis) and a line segment between the fire ellipse
 *  center and the point 'p' determined by \a betaDegrees.
 */
double FBL_SurfaceFireEllipseThetaFromBetaDegrees( double f, double g, double h,
        double betaDegrees )
{
    double betaRadians  = FBL_CompassDegreesToRadians(
                            FBL_CompassConstrainDegrees( betaDegrees ) );
    double thetaRadians = FBL_SurfaceFireEllipseThetaFromBetaRadians( f, g, h,
                            betaRadians );
    double thetaDegrees = FBL_CompassRadiansToDegrees( thetaRadians );
	if ( betaDegrees > 180. )
	{
		thetaDegrees = 360. - thetaDegrees;
	}
    return( thetaDegrees );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse angle 'theta' in radians given 'beta' in radians.
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param g Ellipse parameter g (half the length of the major axis minus the
 *           backing distance). See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param betaRadians Angle (radians) between the direction of maximum fire
 *           spread (the ellipse's major axis) and an arbitrary vector emanating
 *           from the \a ignition \a point.  The arbitrary vector intersects the
 *           fire ellipse perimeter at some point 'p'.
 *
 *  \return Theta, the angle in radians between the direction of maximum spread
 *  (the ellipse's major axis) and a line segment between the fire ellipse
 *  center and the point 'p' determined by \a betaRadians.
 */
double FBL_SurfaceFireEllipseThetaFromBetaRadians( double f, double g, double h,
        double betaRadians )
{
    double cosBeta  = cos( betaRadians );
    double cos2Beta = cosBeta * cosBeta;
    double sin2Beta = 1.0 - cos2Beta;
    double f2 = f * f;
    double g2 = g * g;
    double h2 = h * h;
    double term = sqrt( h2 * cos2Beta + ( f2 - g2 ) * sin2Beta );
    double num  = h * cosBeta * term - ( f * g * sin2Beta );
    double denom = h2 * cos2Beta + f2 * sin2Beta;
    double cosTheta = num / denom;
    double thetaRadians = acos( cosTheta );
    return( thetaRadians );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse angle 'theta' in degrees given 'psi' in degrees.
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param g Ellipse parameter g (half the length of the major axis minus the
 *           backing distance). See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param psiDegrees Psi, the angle in degrees between direction of maximum fire
 *			  spread (the ellipse's major axis) and the normal of the tangent to
 *			  the ellipse at point 'p' as determined by \a thetaRadians.
 *
 *  \return Theta, the angle in degrees between the direction of maximum spread
 *  (the ellipse's major axis) and a line segment between the fire ellipse
 *  center and the point 'p' determined by \a betaDegrees.
 */
double FBL_SurfaceFireEllipseThetaFromPsiDegrees( double f, double h, double psiDegrees )
{
    double psiRadians   = FBL_CompassDegreesToRadians(
                            FBL_CompassConstrainDegrees( psiDegrees ) );
    double thetaRadians = FBL_SurfaceFireEllipseThetaFromPsiRadians( f, h, psiRadians );
    double thetaDegrees = FBL_CompassRadiansToDegrees( thetaRadians );
    return( thetaDegrees );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse angle 'theta' in radians givem 'psi' in radians.
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param psiRadians Psi, the angle in radians between direction of maximum fire
 *			  spread (the ellipse's major axis) and the normal of the tangent to
 *			  the ellipse at point 'p' as determined by \a thetaRadians.
 *
 *  \return Theta, the angle in radians between the direction of maximum spread
 *  (the ellipse's major axis) and a line segment between the fire ellipse
 *  center and the point 'p' determined by \a betaRadians.
 */
double FBL_SurfaceFireEllipseThetaFromPsiRadians( double f, double h, double psiRadians )
{
	double tanThetaRadians = tan( psiRadians ) * h / f;
	double thetaRadians = atan( tanThetaRadians );

	// Adjust for the quadrant
	double boundary1 = 0.5 * M_PI;
	double boundary2 = 1.5 * M_PI;
	if ( psiRadians < boundary1 )
	{
		// no adjustment required
	}
	else if ( psiRadians >= boundary1 && psiRadians < boundary2 )
	{
		thetaRadians += M_PI;
	}
	else if ( psiRadians >= boundary2 )
	{
		thetaRadians += ( 2. * M_PI );
	}
    return( thetaRadians );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse expansion rate at point 'p', the
 *  intersection of the fire ellipse perimeter with a fire spread vector
 *  emanating from the ignition point and at 'beta' degrees from the direction
 *  of maximum fire spread (the ellipse's major axis).
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param g Ellipse parameter g (half the length of the major axis minus the
 *           backing distance). See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param elapsedTime Elapsed time from fire ignition (assuming steady spread rate).
 *  \param betaDegrees Angle (degrees) between the direction of maximum fire
 *           spread (the ellipse's major axis) and an arbitrary vector emanating
 *           from the \a ignition \a point.  The arbitrary vector intersects the
 *           fire ellipse perimeter at some point 'p'.
 *
 *  \return The rate of expansion of the fire ellipse at point 'p' in the
 *  direction normal to the tangent of the ellipse perimeter at point 'p'.
 */
double FBL_SurfaceFireExpansionRateAtBeta( double f, double g, double h,
            double elapsedTime, double betaDegrees )
{
    double thetaDeg = FBL_SurfaceFireEllipseThetaFromBetaDegrees( f, g, h, betaDegrees );
    double rate     = FBL_SurfaceFireExpansionRateAtTheta( f, g, h, thetaDeg,
        elapsedTime );
    return( rate );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse expansion rate at point 'p', the
 *  intersection of the fire ellipse perimeter with a line forming an angle
 *  'psi' with the direction of maximum fire spread (the ellipse's major axis).
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param g Ellipse parameter g (half the length of the major axis minus the
 *           backing distance). See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param elapsedTime Elapsed time from fire ignition (assuming steady spread rate).
 *  \param psiDegrees The angle in degrees between the direction of maximum
 *           fire spread (the ellipse's major axis) and the normal of the
 *           tangent to the ellipse at point 'p'.
 *
 *  \return The rate of expansion of the fire ellipse at point 'p' in the
 *  direction normal to the tangent of the ellipse perimeter at point 'p'.
 */
double FBL_SurfaceFireExpansionRateAtPsi( double f, double g, double h,
            double elapsedTime, double psiDegrees )
{
    if ( elapsedTime < SMIDGEN )
    {
        return( 0.0 );
    }
    double psiRadians = FBL_CompassDegreesToRadians(
                            FBL_CompassConstrainDegrees( psiDegrees ) );
    double cosPsi     = cos( psiRadians );
    double cos2Psi    = cosPsi * cosPsi;
    double sin2Psi    = 1.0 - cos2Psi;
    double fr         = f / elapsedTime;
    double gr         = g / elapsedTime;
    double hr         = h / elapsedTime;
    double term1      = gr * cosPsi;
    double term2      = fr * fr * cos2Psi;
    double term3      = hr * hr * sin2Psi;
    double rate       = term1 + sqrt( term2 + term3 );
    return( rate );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire ellipse expansion rate at point 'p', the
 *  intersection of the fire ellipse perimeter with a line emanating from the
 *  ellipse center and at 'theta' degrees from the direction of maximum fire
 *  spread (the ellipse's major axis)..
 *
 *  \param f Ellipse parameter f (half the length of the major axis)
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param g Ellipse parameter g (half the length of the major axis minus the
 *           backing distance). See Catchpole, de Mestre, and Gill (1982).
 *  \param h Ellipse parameter h (half the length of the minor axis.
 *           See Catchpole, de Mestre, and Gill (1982).
 *  \param elapsedTime Elapsed time from fire ignition (assuming steady spread rate).
 *  \param thetaDegrees Angle (degrees) between the direction of maximum fire
 *           spread (the ellipse's major axis) and an arbitrary vector emanating
 *           from the \a ellipse \a center.  The arbitrary vector intersects the
 *           fire ellipse perimeter at some point 'p'.
 *
 *  \return The rate of expansion of the fire ellipse at point 'p' in the
 *  direction normal to the tangent of the ellipse perimeter at point 'p'.
 */
double FBL_SurfaceFireExpansionRateAtTheta( double f, double g, double h,
            double elapsedTime, double thetaDegrees )
{
    double psiDegrees = FBL_SurfaceFireEllipsePsiFromThetaDegrees( f, h, thetaDegrees );
    double rate       = FBL_SurfaceFireExpansionRateAtPsi( f, g, h,
                            psiDegrees, elapsedTime );
    return( rate );
}

//------------------------------------------------------------------------------
/*! \brief Calculates expected spread rate in random fuels
 *  using Mark Finney's EXRATE algorithms.
 *
 *  \param ros      Pointer to an array of fire spread rates for each fuel type
 *                  (spread rate units of measure are irrelevant).
 *  \param cov      Pointer to an array of fuel coverages for each fuel type
 *                  (coverages are normalized, so can be anything).
 *  \param fuels    Number of fuels in \a ros and \a cov arrays.
 *  \param lbRatio  Fire elliptical length-to-breadth ratio.
 *  \param samples  Number of EXRATE samples (i.e., fuel grid columns).
 *  \param depth    Depth of the EXRATE fuel grid (i.e., rows).
 *  \param laterals Number of lateral extensions to the fuel grid.
 *  \param harmonicRos  Pointer to returned harmonic mean spread rate.
 *
 *  \return Expected fire spread rate through the fuel grid
 *  (in the same units of measure as passed by \a ros).
 */

double FBL_SurfaceFireExpectedSpreadRate( double *ros, double *cov, int fuels,
        double lbRatio, int samples, int depth, int laterals,
        double *harmonicRos )
{
    // Initialize results
    double expectedRos = 0.0;
    if ( harmonicRos )
    {
        *harmonicRos = 0.0;
    }

    // Create a RandFuel instance
    RandFuel randFuel;

    // Mark says the cell size is irrelevant, but he sets it anyway.
    randFuel.setCellDimensions( 10 );

    // Get total fuel coverage
    double totalCov = 0.0;
    int i;
    for ( i=0; i<fuels; i++ )
    {
        totalCov += cov[i];
    }
    // If no fuel coverage, we're done.
    if ( totalCov <= 0.0 )
    {
        return( expectedRos );
    }
    // Allocate the fuels
    if ( ! randFuel.allocFuels( fuels ) )
    {
        return( expectedRos );
    }
    // Normalize fuel coverages and store the fuel ros and cov
    for ( i=0; i<fuels; i++ )
    {
        cov[i] = cov[i] / totalCov;
        randFuel.setFuelData( i, ros[i], cov[i] );
    }
    // Compute the expected and harmonic spread rates
    double maximumRos;
    expectedRos = randFuel.computeSpread2(
            samples,        // columns
            depth,          // rows
            lbRatio,        // fire length-to-breadth ratio
            1,              // always use 1 thread
            &maximumRos,    // returned maximum spread rate
            harmonicRos,    // returned harmonic spread rate
            laterals,       // lateral extensions
            0 );            // less ignitions
    randFuel.freeFuels();

    // Determine expected and harmonic spread rates.
    expectedRos *= maximumRos;
    if ( harmonicRos )
    {
        *harmonicRos *= maximumRos;
    }
    return( expectedRos );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the probability of a firebrand starting a fire.
 *
 *  \param fuelTemperature  Dead surface fuel temperature (oF).
 *  \param fuelMoisture     Dead surface fuel moisture content (lb/lb).
 *
 *  \return Probability of a firebrand starting a fire [0..1].
 */

double FBL_SurfaceFireFirebrandIgnitionProbability( double fuelTemperature,
                double fuelMoisture )
{
    double fuelTemp = ( fuelTemperature - 32. ) * 5. / 9.;
    double qign = 144.51
                - 0.26600 * fuelTemp
                - 0.00058 * fuelTemp * fuelTemp
                - fuelTemp * fuelMoisture
                + 18.5400 * ( 1. - exp( -15.1 * fuelMoisture ) )
                + 640.000 * fuelMoisture;
    if ( qign > 400.0 )
    {
        qign = 400.;
    }

    double x = 0.1 * ( 400. - qign );
    double prob = ( 0.000048 * pow( x, 4.3 ) ) / 50.;
    if ( prob > 1.0 )
    {
        prob = 1.0;
    }
    else if ( prob < 0. )
    {
        prob = 0.0;
    }
    return( prob );
}

//------------------------------------------------------------------------------
/*! \brief Calculates fireline (Byram's) intensity.
 *
 *  \param spreadRate        Fire spread rate (ft/min).
 *  \param reactionIntensity Fire reaction intensity (Btu/ft2/min).
 *  \param residenceTime     Fire residence time (min).
 *
 *  \return Fireline (Byram's) intensity (Btu/ft/s).
 */

double FBL_SurfaceFireFirelineIntensity( double spreadRate,
                double reactionIntensity, double residenceTime )
{
    return( spreadRate * reactionIntensity * residenceTime / 60. );
}

//------------------------------------------------------------------------------
/*! \brief Reverse calculates fireline intensity from flame length.
 *
 *  \param flameLength Flame length (ft)
 *
 *  \return Fireline (Byram's) intensity (Btu/ft/s).
 */

double FBL_SurfaceFireFirelineIntensity( double flameLength )
{
    return( ( flameLength < SMIDGEN )
          ? ( 0.0 )
          : ( pow( ( flameLength / 0.45 ), ( 1. / 0.46 ) ) ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the Psi version of the Beta fireline intensity
 *	by scaling it by the ratio of the psi-to-beta spread rates.
 *
 *  \param ros_psi Rate of spread at angle Psi (ft/min)
 *  \param ros_beta Rate of spread at angle Beta (ft/min)
 *	\param fli_beta Fireline intensity at angle Beta (Btu/ft/s)
 *
 *  \return Fireline (Byram's) intensity at angle Psi (Btu/ft/s).
 */

double FBL_SurfaceFireFirelineIntensityAtPsi( double ros_psi, double ros_beta,
	double fli_beta )
{
	return ( ros_beta < SMIDGEN || fli_beta < SMIDGEN )
		? ( 0.0 )
		: fli_beta * ros_psi / ros_beta;
}
//------------------------------------------------------------------------------
/*! \brief Calculates flame length from fireline (Byram's) intensity.
 *
 *  \param firelineIntensity Fireline (Byram's) intensity (btu/ft/s).
 *
 *  \return Flame length (ft).
 */

double FBL_SurfaceFireFlameLength( double firelineIntensity )
{
    return( ( firelineIntensity < SMIDGEN )
          ? ( 0.0 )
          : ( 0.45 * pow( firelineIntensity, 0.46 ) ) );
}

#ifdef INCLUDE_V5_CODE
//------------------------------------------------------------------------------
/*! \brief Calculates the fire's forward spread rate in the direction of
 *  maximum spread.
 *
 *  \param noWindNoSlopeSpreadRate Fire spread rate without any wind or slope
 *                            effects (ft/min).
 *  \param slopeFraction      Terrain slope steepness (rise/reach).
 *  \param windDirFromUplope  Wind vector direction ( degrees clockwise from
 *                            upslope).
 *  \param midflameWindSpeed  Wind speed at midflame height (mi/h).
 *
 *  \param[out] maxDirFromUpslope Address where the fire's direction of maximum
 *                            spread is returned (degrees clockwise from upslope).
 *  \param[out] effectiveWindSpeed Address where the effective wind speed is
 *                            returned (mi/h).
 *  \param[out] windSpeedLimit Address where the upper reliable limit of wind
 *                             speed is returned (mi/h).
 *  \param[out] windLimitExceeded Address where the wind limit exceeded flag is
 *                            returned (0==not exceeded, 1==exceeded).
 *  \param[out] windFactor    Address where the wind factor is returned (dl).
 *  \param[out] slopeFactor   Address where the slope factor is returned (dl).
 *  \param applyWindLimit     If TRUE, the maximum reliable wind speed limit is
 *                            applied to spread rate.
 *
 *  \return Fire's forward (heading) spread rate in the direction of maximum
 *  spread (ft/min).
 */

double FBL_SurfaceFireForwardSpreadRate(
            double noWindNoSlopeSpreadRate,
            double reactionIntensity,
            double slopeFraction,
            double midflameWindSpeed,
            double windDirFromUpslope,
            double *maxDirFromUpslope,
            double *effectiveWindSpeed,
            double *windSpeedLimit,
            int    *windLimitExceeded,
            double *windFactor,
            double *slopeFactor,
            bool    applyWindLimit )
{
    // Slope factor
    double phiS  = m_slopeK * slopeFraction * slopeFraction;

    // Wind factor
    double windFpm = 88. * midflameWindSpeed;
    double phiW  = ( windFpm < SMIDGEN )
                 ? ( 0.0 )
                 : ( m_windK * pow( windFpm, m_windB ) );

    // Combined wind-slope factor
    double phiEw = phiS + phiW;

    // Wind direction relative to upslope
    double windDir = windDirFromUpslope;

    // No-wind no-slope spread rate and parameters
    double ros0      = noWindNoSlopeSpreadRate;
    double rosMax    = ros0;
    double dirMax    = 0.;
    double effWind   = 0.;
    int    doEffWind = 0;
    int    windLimit = 0;
    int    situation = 0;
    // Situation 1: no fire spread.
    if ( ros0 < SMIDGEN )
    {
        rosMax    = ros0;
        dirMax    = 0;
        effWind   = 0.;
        // There IS an effective wind even if there is no fire.
        doEffWind = 1;
        // But since BEHAVE doesn't calculate effective wind when no spread,
        // we wont either.
        doEffWind = 0;
        situation = 1;
    }
    // Situation 2: no wind and no slope.
    else if ( phiEw < SMIDGEN )
    {
        rosMax    = ros0;
        dirMax    = 0;
        effWind   = 0.;
        doEffWind = 0;
        situation = 2;
    }
    // Situation 3: wind with no slope.
    else if ( phiS < SMIDGEN )
    {
        rosMax    = ros0 * (1. + phiEw);
        dirMax    = windDir;
        effWind   = windFpm;
        doEffWind = 0;
        situation = 3;
    }
    // Situation 4: slope with no wind.
    else if ( phiW < SMIDGEN )
    {
        rosMax    = ros0 * (1. + phiEw);
        dirMax    = 0.;
        doEffWind = 1;
        situation = 4;
    }
    // Situation 5: wind blows upslope.
    else if ( windDir < SMIDGEN )
    {
        rosMax    = ros0 * (1. + phiEw);
        dirMax    = 0.;
        doEffWind = 1;
        situation = 5;
    }
    // Situation 6: wind blows cross slope.
    else
    {
        // Calculate spread rate in optimal direction.
        double splitRad  = windDir * M_PI / 180.;
        double slpRate   = ros0 * phiS;
        double wndRate   = ros0 * phiW;
        double x         = slpRate + wndRate * cos(splitRad);
        double y         = wndRate * sin(splitRad);
        double rv        = sqrt(x*x + y*y);
        rosMax = ros0 + rv;

        // Recalculate phiEw in the optimal direction.
        phiEw     = ( ros0 < SMIDGEN )
                  ? ( 0.0 )
                  : ( rosMax / ros0 - 1.0 );
        doEffWind = ( phiEw > SMIDGEN )
                  ? ( 1 )
                  : ( 0 );
        // Recalculate direction of maximum spread in azimuth degrees.
        double al = ( rv < SMIDGEN )
                  ? ( 0.0 )
                  : ( asin( fabs( y ) / rv ) );
        double a;
        if ( x >= 0. )
        {
            a = ( y >= 0. )
              ? ( al )
              : ( M_PI + M_PI - al );
        }
        else
        {
            a = ( y >= 0. )
              ? ( M_PI - al )
              : ( M_PI + al );
        }
        dirMax = a * 180. / M_PI;
        if ( fabs( dirMax ) < 0.5 )
        {
            dirMax = 0.0;
        }
        situation = 6;
    }
    // Recalculate effective wind speed based upon phiEw.
    if ( doEffWind )
    {
        effWind = ( ( phiEw * m_windE ) < SMIDGEN || m_windB < SMIDGEN )
                ? ( 0.0 )
                : ( pow( ( phiEw * m_windE ), ( 1. / m_windB ) ) );
    }
    // If effective wind exceeds maximum wind, scale back spread & phiEw.
    double maxWind = 0.9 * reactionIntensity;
    if ( effWind > maxWind )
    {
        windLimit = 1;
        if ( applyWindLimit )
        {
            phiEw     = ( maxWind < SMIDGEN )
                      ? ( 0.0 )
                      : ( m_windK * pow( maxWind, m_windB ) );
            rosMax    = ros0 * ( 1. + phiEw );
            effWind   = maxWind;
        }
    }
	// Added to BehavePlus6 by PLA
	if ( rosMax > effWind && effWind > 88. )
	{
		rosMax = effWind;
	}
    // Return results
    *maxDirFromUpslope  = dirMax;
    *effectiveWindSpeed = effWind / 88.;
    *windSpeedLimit     = maxWind / 88.;
    *windLimitExceeded  = windLimit;
    *windFactor         = phiW;
    *slopeFactor        = phiS;
    //fprintf( stderr, "maxDirFromUpslope=%f\n", dirMax );
    //fprintf( stderr, "effectiveWindSpeed=%f\n", *effectiveWindSpeed );
    //fprintf( stderr, "windSpeedLimit=%f\n", *windSpeedLimit );
    //fprintf( stderr, "windLimitExceeded=%d\n", windLimit);
    //fprintf( stderr, "rosMax=%f\n", rosMax );
    return( rosMax );
}
#endif

//------------------------------------------------------------------------------
/*! \brief Calculates the fire's heat per unit area.
 *
 *  \param reactionIntensity Fire reaction intensity (Btu/ft2/min).
 *  \param residenceTime     Fire residence time (min).
 *
 *  \return Fire's heat per unit area (Btu/ft2).
 */

double FBL_SurfaceFireHeatPerUnitArea( double reactionIntensity,
                double residenceTime )
{
    return( reactionIntensity * residenceTime );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire's heat source (reaction intensity
 *  plus wind-slope effect).
 *
 *  \param spreadRateAtHead Maximum spread rate at fire head (ft/min).
 *  \param heatSink     Fire's fuel bed heat sink (btu/ft3).
 *
 *  \return Fire heat source (propagating flux) (btu/ft2/min).
 */

double FBL_SurfaceFireHeatSource( double spreadRateAtHead, double heatSink )
{
    return( spreadRateAtHead * heatSink );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire length-to-width ratio given the effective
 *  wind speed (combined wind-slope effect).
 *
 *  \param effectiveWindSpeed Effective wind speed at fire's head (mi/h).
 *
 *  \return Fire length-to-width ratio (ft/ft).
 */

double FBL_SurfaceFireLengthToWidthRatio( double effectiveWindSpeed )
{
    return( ( effectiveWindSpeed > SMIDGEN )
          ? ( 1. + 0.25 * effectiveWindSpeed )
          : ( 1. ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the probability of a lightning strike starting a fire.
 *
 *  \param fuelType Ignition fuel bed type:
 *                      0 == Ponderosa Pine Litter
 *                      1 == Punky wood, rotten, chunky
 *                      2 == Punky wood powder, deep (4.8 cm)
 *                      3 == Punk wood powder, shallow (2.4 cm)
 *                      4 == Lodgepole pine duff
 *                      5 == Douglas-fir duff
 *                      6 == High altitude mixed (mainly Engelmann spruce)
 *                      7 == Peat moss (commercial)
 *  \param depth    Ignition fuel bed depth (inches).
 *  \param moisture Ignition fuel moisture content (lb/lb).
 *  \param charge   Lightning charge:
 *                      0 == negative,
 *                      1 == positive,
 *                      2 == unknown
 *
 *  \note  The following assumptions are made by Latham:
 *      - 20% of negative flashes have continuing current
 *      - 90% of positive flashes have continuing current
 *      - Latham and Schlieter found a relative frequency of
 *          0.723 negative and 0.277 positive strikes
 *      - Unknown strikes are therefore p = 0.1446 neg + 0.2493 pos
 *
 *  \return Probability of the lightning strike starting a fire [0..1].
 */

double FBL_SurfaceFireLightningIgnitionProbability( int fuelType, double depth,
            double moisture, int charge )
{
    // Probability of continuing current by charge type (Latham)
    static const double ccNeg = 0.2;
    static const double ccPos = 0.9;

    // Relative frequency by charge type (Latham and Schlieter)
    static const double freqNeg = 0.723;
    static const double freqPos = 0.277;

    // Convert duff depth to cm and restrict to maximum of 10 cm.
    depth *= 2.54;
    if ( depth > 10. )
    {
        depth = 10.;
    }
    // Convert duff moisture to percent and restrict to maximum of 40%.
    moisture *= 100.;
    if ( moisture > 40. )
    {
        moisture = 40.;
    }
    // Ponderosa Pine Litter
    double pPos = 0.;
    double pNeg = 0.;
    double prob = 0.;
    if ( fuelType == 0 )
    {
        pPos = 0.92 * exp( -0.087 * moisture );
        pNeg = 1.04 * exp( -0.054 * moisture );
    }
    // Punky wood, rotten, chunky
    else if ( fuelType == 1 )
    {
        pPos = 0.44 * exp( -0.110 * moisture );
        pNeg = 0.59 * exp( -0.094 * moisture );
    }
    // Punky wood powder, deep (4.8 cm)
    else if ( fuelType == 2 )
    {
        pPos = 0.86 * exp( -0.060 * moisture );
        pNeg = 0.90 * exp( -0.056 * moisture );
    }
    // Punk wood powder, shallow (2.4 cm)
    else if ( fuelType == 3 )
    {
        pPos = 0.60 - ( 0.011 * moisture );
        pNeg = 0.73 - ( 0.011 * moisture );
    }
    // Lodgepole pine duff
    else if ( fuelType == 4 )
    {
        pPos = 1. / ( 1. + exp( 5.13 - 0.68 * depth ) );
        pNeg = 1. / ( 1. + exp( 3.84 - 0.60 * depth ) );
    }
    // Douglas-fir duff
    else if ( fuelType == 5 )
    {
        pPos = 1. / ( 1. + exp( 6.69 - 1.39 * depth ) );
        pNeg = 1. / ( 1. + exp( 5.48 - 1.28 * depth ) );
    }
    // High altitude mixed (mainly Engelmann spruce)
    else if ( fuelType == 6 )
    {
        pPos = 0.62 * exp( -0.050 * moisture );
        pNeg = 0.80 - ( 0.014 * moisture );
    }
    // Peat moss (commercial)
    else if ( fuelType == 7 )
    {
        pPos = 0.71 * exp( -0.070 * moisture );
        pNeg = 0.84 * exp( -0.060 * moisture );
    }
    // Return requested result
    static const int negative = 0, positive = 1, unknown = 2;
    if ( charge == negative )
    {
        prob = ccNeg * pNeg;
    }
    else if ( charge == positive )
    {
        prob = ccPos * pPos;
    }
    else if ( charge == unknown )
    {
        prob = freqPos * ccPos * pPos
             + freqNeg * ccNeg * pNeg;
    }
    // Constrain result
    if ( prob < 0. )
    {
        prob = 0.;
    }
    if ( prob > 1.0 )
    {
        prob = 1.0;
    }
    return( prob );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire no-wind, no-slope spread rate from the ratio of
 *  the heat source to heat sink.
 *
 *  \param reactionIntensity Fire total reaction intensity (Btu/ft2/min).
 *  \param propagatingFlux   Fire propagating flux (fraction).
 *  \param heatSink          Fire total heat sink (Btu/ft3).
 *
 *  \return No-wind, no-slope spread rate (ft/min).
 */

double FBL_SurfaceFireNoWindNoSlopeSpreadRate( double reactionIntensity,
                double propagatingFlux, double heatSink )
{
    double ros0 = ( heatSink < SMIDGEN )
                ? ( 0.0 )
                : ( reactionIntensity * propagatingFlux / heatSink );
    //fprintf( stderr, "ros0=%f\n", ros0 );
    return( ros0 );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire perimeter from its length and width.
 *
 *  \param fireLength   Fire ellipse length (ft).
 *  \param fireWidth    Fire ellipse width (ft).
 *
 *  \return Fire perimeter (ft).
 */

double FBL_SurfaceFirePerimeter( double fireLength, double fireWidth )
{
    double a = 0.5 * fireLength;
    double b = 0.5 * fireWidth;
    double xm = ( ( a + b ) < SMIDGEN )
              ? ( 0. )
              : ( ( a - b ) / ( a + b ) );
    double xk = 1. + xm * xm / 4. + xm * xm * xm * xm / 64.;
    double perim = M_PI * ( a + b ) * xk;
    return( perim );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire's propagating flux.
 *
 *  \param fuelBedPackingRatio Surface fuel bed packing ratio (beta) (ft3/ft3).
 *  \param fuelBedSavr         Surface fuel bed's characteristic surface area-
 *                             to-volume ratio (sigma) (ft2/ft3).
 *
 *  \return Fire's propagating flux (fraction).
 */

double FBL_SurfaceFirePropagatingFlux( double fuelBedPackingRatio,
                double fuelBedSavr )
{
    double propagatingFlux = ( fuelBedSavr < SMIDGEN)
          ? ( 0. )
          : ( exp( ( 0.792 + 0.681 * sqrt( fuelBedSavr ) )
            * ( fuelBedPackingRatio + 0.1 ) )
            / ( 192. + 0.2595 * fuelBedSavr ) );
    //fprintf( stderr, "propagatingFule=%f\n", propagatingFlux );
    return( propagatingFlux );
}

#ifdef INCLUDE_V5_CODE
//------------------------------------------------------------------------------
/*! \brief Calculates the fire's reaction intensity from the dead and live
 *  fuel moisture contents and extinction moisture contents.
 *
 *  \par Note that FBL_SurfaceFuelBedIntermediates() must have been previously
 *  called to determine intermediate variables used by this function.
 *
 *  \param deadMois Dead surface fuel bed moisture content (lb/lb).
 *  \param deadMext Dead surface fuel bed extinction moisture content (lb/lb).
 *  \param liveMois Live surface fuel bed moisture content (lb/lb).
 *  \param liveMext Live surface fuel bed extinction moisture content (lb/lb).
 *  \parm[out] deadRxInt Pointer to storage for returned dead fuel reaction
 *              intensity (Btu/ft2/min).
 *  \parm[out] liveRxInt Pointer to storage for returned dead fuel reaction
 *              intensity (Btu/ft2/min).
 *
 *  \return Fire reaction intensity (Btu/ft2/min).
 */

double FBL_SurfaceFireReactionIntensity( double deadMois, double deadMext,
                double liveMois, double liveMext,
                double* deadRxInt, double* liveRxInt )
{
    //  Moisture damping coefficient by life class.
    double r;
    double deadEtaM = ( deadMext < SMIDGEN || (r = deadMois / deadMext) >= 1. )
                      ? ( 0.0 )
                      : ( 1.0 - 2.59*r + 5.11*r*r - 3.52*r*r*r );

    double liveEtaM = ( liveMext < SMIDGEN || (r = liveMois / liveMext) >= 1. )
                      ? ( 0.0 )
                      : ( 1.0 - 2.59*r + 5.11*r*r - 3.52*r*r*r) ;

    // Combine moisture damping with rx factor to get total reaction int.
    double rxIntDead = m_lifeRxK[0] * deadEtaM;
    double rxIntLive = m_lifeRxK[1] * liveEtaM;
    double rxInt     = rxIntDead + rxIntLive;

    // Store optional outputs
    if ( deadRxInt > 0 )
    {
        *deadRxInt = rxIntDead;
    }
    if ( liveRxInt > 0 )
    {
        *liveRxInt = rxIntLive;
    }
    return( rxInt );
}
#endif

//------------------------------------------------------------------------------
/*! \brief Calculates the fire's residence time.
 *
 *  \param fuelBedSavr  Surface fuel bed characteristic surface area-to-volume
 *                      ratio (sigma) (ft2/ft3).
 *
 *  \return Fire residence time (min).
 */

double FBL_SurfaceFireResidenceTime( double fuelBedSavr )
{
    return( ( fuelBedSavr < SMIDGEN )
          ? ( 0.0 )
          : ( 384. / fuelBedSavr ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates scorch height from fireline intensity, wind speed, and
 *  air temperature.
 *
 *  \param firelineIntensity Fireline (Byram's) intensity (btu/ft/s).
 *  \param windSpeed         Wind speed at midlame height (m/h).
 *  \param airTemperature    Air temperature (oF).
 *
 *  \return Scorch height (ft).
 */

double FBL_SurfaceFireScorchHeight( double firelineIntensity, double windSpeed,
    double airTemperature )
{
    return( ( firelineIntensity < SMIDGEN )
          ? ( 0.0 )
          : ( ( 63. / ( 140. - airTemperature ) )
            * pow( firelineIntensity, 1.166667 )
            / sqrt( firelineIntensity + ( windSpeed * windSpeed * windSpeed ) )
            ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the angle \b beta between the direction of maximum
 *  spread and a direction of interest (bothrelative to upslope).
 *
 *  \param maxSpreadDirFromUpslope  Direction of maximum fire spread
 *                                  (degrees clockwise from upslope).
 *  \param vectorDirFromUpslope     Fire spread vector of interest
 *                                  (degrees clockwise from upslope).
 *
 *  \return Absolute value of the angle between the two vectors (degrees).
 */

double FBL_SurfaceFireVectorBeta(
            double maxSpreadDirFromUpslope, double vectorDirFromUpslope )
{
    double beta = fabs( maxSpreadDirFromUpslope - vectorDirFromUpslope );
	return beta;
    //return( ( beta > 180. ) ? ( 360. - beta ) : beta );
}


//------------------------------------------------------------------------------
/*! \brief Calculates the fire spread rate at 'beta' degrees from the
 *  direction of maximum spread.
 *
 *  \param forwardSpreadRate Fire spread rate in the direction of maximum
 *                           spread (ft/min).
 *  \param eccentricity      Fire eccentricity (ft/ft).
 *  \param beta              Fire spread vector of interest (degrees clockwise
 *                           from direction of maximum spread).
 *
 *  \return Fire spread rate along the specified vector (ft/min).
 */
double FBL_SurfaceFireSpreadRateAtBeta( double forwardSpreadRate,
            double eccentricity, double beta )
{
    double rosVec = forwardSpreadRate;
    // Calculate the fire spread rate in this azimuth
    // if it deviates more than a tenth degree from the maximum azimuth
    if ( fabs( beta ) > 0.1 )
    {
        double radians = FBL_CompassDegreesToRadians( beta );
        rosVec = forwardSpreadRate * ( 1. - eccentricity )
               / ( 1. - eccentricity * cos( radians ) );
    }
    return( rosVec );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the fire spread rate at a direction relative to upslope.
 *
 *  \param forwardSpreadRate        Fire spread rate in the direction of
 *                                  maximum spread (ft/min).
 *  \param maxSpreadDirFromUpslope  Direction of maximum fire spread
 *                                  (degrees clockwise from upslope).
 *  \param eccentricity             Fire eccentricity (ft/ft).
 *  \param vectorDirFromUpslope     Fire spread vector of interest
 *                                  (degrees clockwise from upslope).
 *
 *  \return Fire spread rate along the specified vector (ft/min).
 */

double FBL_SurfaceFireVectorSpreadRate( double forwardSpreadRate,
            double maxSpreadDirFromUpslope, double eccentricity,
            double vectorDirFromUpslope )
{
    double beta = FBL_SurfaceFireVectorBeta( maxSpreadDirFromUpslope,
                        vectorDirFromUpslope );
    return( FBL_SurfaceFireSpreadRateAtBeta( forwardSpreadRate, eccentricity,
        beta) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates fire maximum width from its length and length-to-width
 *  ratio.
 *
 *  \param fireLength       Fire length (ft).
 *  \param lengthWidthRatio Fire length-to-width ratio (ft/ft).
 *
 *  \return Maximum fire width (ft).
 */

double FBL_SurfaceFireWidth( double fireLength, double lengthWidthRatio )
{
    return( ( lengthWidthRatio < SMIDGEN )
          ? ( 0.0 )
          : ( fireLength / lengthWidthRatio ) );
}

#ifdef INCLUDE_V5_CODE
//------------------------------------------------------------------------------
/*! \brief Calculates the fuel bed heat sink from the current fuel bed
 *  intermediates and moisture conditions.
 *
 *  \param bulkDensity  Fuel bed bulk density (lb/ft3).
 *  \param deadMext     Dead fuel extinction moisture content (lb/lb).
 *  \param mois         Array of size m_particles containing fuel particle
 *                      moisture contents (lb/lb).
 *  \param deadFuelMois Address of returned dead category fuel moisture (lb/lb).
 *  \param liveFuelMois Address of returned live category fuel moisture (lb/lb).
 *  \param liveFuelMext Address of returned live category fuel extinction
 *                      moisture content (lb/lb).
 *
 *  \return Fuel bed heat sink (Btu/ft3).
 */

double FBL_SurfaceFuelBedHeatSink( double bulkDensity, double deadMext,
            double *mois, double *deadFuelMois, double *liveFuelMois,
            double *liveFuelMext )
{
    // Calculate results
    int    l, p;
    double qig = 0.;
    double rbQig = 0.;
    int    nLive = 0;
    double fdmois = 0.;
    double wfmd = 0.;
    double deadMois = 0.;
    double liveMois = 0.;
    // Compute category weighted moisture and accumulate the rbQig.
    for ( p = 0;
          p < m_particles;
          p++ )
    {
        qig = 250. + 1116. * mois[p];
        l = FBL_FuelLife( m_life[p] );
        if ( l == DEAD_CAT )
        {
            wfmd     += mois[p] * m_sigK[p] * m_load[p];
            deadMois += m_aWtg[p] * mois[p];
        }
        else
        {
            nLive++;
            liveMois += m_aWtg[p] * mois[p];
        }
		// Note that m_sigK[p] is the effective heating number of particle p
        rbQig += qig * m_aWtg[p] * m_lifeAwtg[l] * m_sigK[p];
    }
    // Complete the rbQig calculation.
    rbQig *= bulkDensity;
    // Compute live fuel extinction moisture.
    double liveMext = deadMext;
    if ( nLive )
    {
        fdmois = ( m_lifeFine[DEAD_CAT] < SMIDGEN )
               ? ( 0.0 )
               : ( wfmd / m_lifeFine[DEAD_CAT] );

        liveMext = ( deadMext < SMIDGEN )
                 ? ( 0.0 )
                 : ( ( m_liveMextK * ( 1.0 - fdmois / deadMext ) ) - 0.226 );
    }
    liveMext = ( liveMext < deadMext )
             ? ( deadMext )
             : ( liveMext );
    // Hack requested by Pat Andrews
    //if ( liveMext > 4.00 )
    //{
    //    liveMext = 4.00;
    //}
    // Return results
    *deadFuelMois = deadMois;
    *liveFuelMois = liveMois;
    *liveFuelMext = liveMext;
    //fprintf( stderr, "heatSink = %f\n", rbQig );
    return( rbQig );
}
#endif

#ifdef INCLUDE_V5_CODE
//------------------------------------------------------------------------------
/*! \brief Calculates surface fuel bed intermediate variables that depend only
 *  on fuel bed arrangement and particle properties.
 *
 *  \param depth     Fuel bed depth (ft).
 *  \param deadMext  Dead fuel extinction moisture content (lb/lb).
 *  \param particles Number of fuel particles
 *  \param life      Array of fuel particle life codes (0==dead, 1=live).
 *  \param load      Array of fuel particle loads (lbs/ft2).
 *  \param savr      Array of fuel particle surface area-to-volume ratios
 *                   (ft2/ft3).
 *  \param heat      Array of fuel particle heat of combustions (Btu/lb).
 *  \param dens      Array of fuel particle densities (lbs/ft3).
 *  \param stot      Array of fuel particle total silica contents (lbs Si/lb fuel).
 *  \param seff      Array of fuel particle effective silica contents (lb Si/lb fuel).
 *
 *  \param fuelBedBulkDensity   Address of returned fuel bed bulk density
 *                              (lbs/ft3).
 *  \param fuelBedPackingRatio  Address of returned fuel bed packing ratio
 *                              (lb3/lb3).
 *  \param fuelBedBetaRatio     Address of returned fuel bed packing ratio-to-
 *                              optimum packing ratio (fraction).
 *
 *  \return Fuel bed characteristic surface area-to-volume ratio (ft2/ft3).
 */

int FBL_FuelLife( int lifeCode )
{
    static int Life[4] =
    {
        // FuelLifeType_DeadTimeLag=0, Dead category, dead time lag size class moisture
        DEAD_CAT,
        // FuelLifeType_LiveHerb=1, Live category, live herbaceous moisture
        LIVE_CAT,
        // FuelLifeType_LiveWood=2, Live category, live woody moisture
        LIVE_CAT,
        // FuelLifeType_DeadLitter=3, Dead category, 100-h time lag moisture
        DEAD_CAT
    };
    return( Life[ lifeCode ] );
}

double FBL_SurfaceFuelBedIntermediates(
            double depth, double /* deadMext */, int particles,
            int *life, double *load, double *savr, double *heat, double *dens,
            double *stot, double *seff, double *fuelBedBulkDensity,
            double *fuelBedPackingRatio, double *fuelBedBetaRatio )
{
    // Savr by size class                 .04"  .25"  0.5"  1.0"  3.0"
    static double Size_bdy[MAX_SIZES] = { 1200., 192., 96.0, 48.0, 16.0, 0. };
    //static double Size_bdy[MAX_SIZES] = { 192., 48.0, 16.0, 0. };
    int l, p, s;
    double c, e, beta, betaOpt, aa, sigma15, gammaMax, gamma;
    // Particle intermediates.
    int    size[MAX_PARTS];
    double area[MAX_PARTS];
    double sWtg[MAX_PARTS];
    // Life category intermediates.
    double lifeArea[MAX_CATS];
    double lifeEtaS[MAX_CATS];
    double lifeHeat[MAX_CATS];
    double lifeLoad[MAX_CATS];
    double lifeSavr[MAX_CATS];
    double lifeSeff[MAX_CATS];
    double lifeStot[MAX_CATS];
    double lifeSwtg[MAX_CATS][MAX_SIZES];
    // Fuel bed intermediates output by this function
    double betaRatio    = 0.;
    double bulkDensity  = 0.;
    double packingRatio = 0.;
    double sigma        = 0.;
    double totalArea    = 0.;
    double totalLoad    = 0.;
    // Initialize stored values
    m_particles = particles;
    for ( p = 0;
          p < m_particles;
          p++ )
    {
        size[p]   = 0;
        m_life[p] = 0;
        m_load[p] = 0.0;
    }
    // Initialize all accumulated variables.
    for ( l = 0;
          l < MAX_CATS;
          l++)
    {
        lifeArea[l]   = 0.;
        m_lifeAwtg[l] = 0.;
        lifeEtaS[l]   = 0.;
        lifeHeat[l]   = 0.;
        m_lifeFine[l] = 0.;
        lifeLoad[l]   = 0.;
        m_lifeRxK[l]  = 0.;
        lifeSavr[l]   = 0.;
        lifeSeff[l]   = 0.;
        lifeStot[l]   = 0.;
        for ( s = 0;
              s < MAX_SIZES;
              s++ )
        {
            lifeSwtg[l][s] = 0.;
        }
    }
    m_liveMextK = 0.0;
    // If no fuel bed depth, we're done
    if ( depth < SMIDGEN )
    {
        goto done;
    }
    // Store life and load for later use
    for ( p = 0;
          p < m_particles && p < MAX_PARTS;
          p++ )
    {
        m_life[p] = life[p];
        m_load[p] = load[p];
    }
    // Determine particle area, savr exponent factor, and size class.
    for ( p = 0;
          p < m_particles;
          p++ )
    {
        // Accumulate fuel bed total load
        totalLoad += m_load[p];
        // Particle surface area
        area[p] = ( dens[p] < SMIDGEN )
                ? ( 0.0 )
                : ( m_load[p] * savr[p] / dens[p] );
        // Accumulate fuel bed and life category surface areas
        l = FBL_FuelLife( m_life[p] );
        lifeArea[l] += area[p];
        totalArea += area[p];
        // Fuel bed packing ratio
        if ( dens[p] > SMIDGEN )
        {
            packingRatio += m_load[p] / dens[p];
        }

        m_sigK[p] = ( savr[p] < SMIDGEN )
                  ? ( 0.0 )
                  : ( exp( -138. / savr[p] ) );
        // Determine size class for this particle
        for ( s = 0;
              savr[p] < Size_bdy[s];
              s++ )
        {
            ; // NOTHING
        }
        size[p] = s;
    }
    // If there is no fuel area, then we're done.
    if ( totalArea < SMIDGEN )
    {
        goto done;
    }
    // Complete the bulkDensity, packingRatio, and slopeK calculations.
    if ( depth > SMIDGEN )
    {
        bulkDensity = totalLoad / depth;
        packingRatio /= depth;
    }
    m_slopeK = ( packingRatio < SMIDGEN )
             ? ( 0.0 )
             : ( 5.275 * pow( packingRatio, -0.3 ) );
    // Surface area wtg factor for each particle within its life category
    // and within its size class category (used to weight loading).
    for ( p = 0;
          p < m_particles;
          p++ )
    {
        l = FBL_FuelLife( m_life[p] );
        m_aWtg[p] = ( lifeArea[l] < SMIDGEN )
                  ? ( 0.0 )
                  : ( area[p] / lifeArea[l] );
        lifeSwtg[l][size[p]] += m_aWtg[p];
    }
    // Assign size class surface area weights to each particle.
    for ( p = 0;
          p < m_particles;
          p++ )
    {
        l = FBL_FuelLife( m_life[p] );
        sWtg[p] = lifeSwtg[l][size[p]];
    }
    // Derive life category surface area weighting factors.
    for ( l = 0;
          l < MAX_CATS;
          l++ )
    {
        m_lifeAwtg[l] = ( totalArea < SMIDGEN )
                      ? ( 0.0 )
                      : ( lifeArea[l] / totalArea );
    }
    // Accumulate life category weighted load, savr, heat, seff, and stot,
    // and the particle's contribution to bulk density and packing ratio.
    for ( p = 0;
          p < m_particles;
          p++ )
    {
        l = FBL_FuelLife( m_life[p] );
        lifeLoad[l] += sWtg[p] * m_load[p];
        lifeSavr[l] += m_aWtg[p] * savr[p];
        lifeHeat[l] += m_aWtg[p] * heat[p];
        lifeSeff[l] += m_aWtg[p] * seff[p];
        lifeStot[l] += m_aWtg[p] * stot[p];
    }
    // Fuel bed characteristics surface area-to-volume (sigma).
    for ( l = 0;
          l < MAX_CATS;
          l++ )
    {
        //fprintf( stderr, "lifeSavr[%d]=%f, m_lifeAwtg[%d]=%f\n",
        //l, lifeSavr[l], l, m_lifeAwtg[l] );
        sigma += m_lifeAwtg[l] * lifeSavr[l];
    }
    // Optimum reaction velocity computations.
    beta      = packingRatio;
    betaOpt   = 3.348 / ( pow( sigma, 0.8189 ) );
    aa        = 133. / ( pow( sigma, 0.7913 ) );
    sigma15   = pow( sigma, 1.5 );
    gammaMax  = sigma15 / ( 495. + 0.0594 * sigma15 );
    betaRatio = ( betaOpt < SMIDGEN )
              ? ( 0.0 )
              : ( beta / betaOpt );
    if ( betaRatio > SMIDGEN && betaRatio != 1. )
    {
        gamma = gammaMax * pow( betaRatio, aa ) * exp( aa * ( 1. - betaRatio ) );
    }
    // Slope and wind fuel bed intermediates.
    m_windB  = 0.02526 * pow( sigma, 0.54 );
    c        = 7.47 * exp( -0.133 * pow( sigma, 0.55 ) );
    e        = 0.715 * exp( -0.000359 * sigma );
    m_windK  = ( betaRatio < SMIDGEN )
             ? ( 0. )
             : ( c * pow( betaRatio, -e ) );
    m_windE  = ( betaRatio < SMIDGEN || c < SMIDGEN )
             ? ( 0. )
             : ( pow( betaRatio, e ) / c );
    // Life category mineral damping coefficient
    // and contribution to reaction intensity.
    for ( l = 0;
          l < MAX_CATS;
          l++ )
    {
        // Mineral damping coefficient.
        if ( ( lifeEtaS[l] = ( lifeSeff[l] < SMIDGEN )
                    ? ( 1.0 )
                    : ( 0.174 / pow( lifeSeff[l], 0.19 ) )
             ) > 1.0 )
        {
            lifeEtaS[l] = 1.0;
        }
        m_lifeRxK[l] = gamma * lifeEtaS[l] * lifeHeat[l] * lifeLoad[l]
                     * ( 1. - lifeStot[l] );
    }
    //  Fine dead and fine live fuel factors.
    for ( p = 0;
          p < m_particles;
          p++ )
    {
        l = FBL_FuelLife( m_life[p] );
        if ( l == DEAD_CAT )
        {
            m_lifeFine[l] += m_load[p] * m_sigK[p];
        }
        else if ( savr[p] > SMIDGEN )
        {
            m_lifeFine[l] += m_load[p] * exp( -500. / savr[p] );
        }
    }
    // Live fuel extinction moisture factor.
    m_liveMextK = ( m_lifeFine[LIVE_CAT] < SMIDGEN )
                ? ( 0.0 )
                : ( 2.9 * m_lifeFine[DEAD_CAT] / m_lifeFine[LIVE_CAT] );
    // Store results
    done:
    *fuelBedBulkDensity  = bulkDensity;
    *fuelBedPackingRatio = packingRatio;
    *fuelBedBetaRatio    = betaRatio;
    //fprintf( stderr, "bulkDensity=%f\n", bulkDensity );
    //fprintf( stderr, "packingRatio=%f\n", packingRatio );
    //fprintf( stderr, "betaRatio=%f\n", betaRatio );
    //fprintf( stderr, "sigma=%f\n", sigma );
    return( sigma );
}
#endif

//------------------------------------------------------------------------------
/*! \brief Calculates the fuel temperature using the BEHAVE FIRE2 subroutine
 *  CAIGN() algorithm.
 *
 *  \param airTemperature Air temperature (oF).
 *  \param sunShade       Fraction of sun shaded from the fuel.
 *
 *  \return Fuel temperature (oF).
 */

double FBL_SurfaceFuelTemperature( double airTemperature, double sunShade )
{
    // FIRE2 SUBROUTINE CAIGN() restricts air temp to 5-degree intervals
    int iAirTemp = (int) ( airTemperature / 10. );
    double airTemp = 5. + ( 10. * iAirTemp );

    // But we are going to use the continuum
    airTemp= airTemperature;

    // Temperature differential depends upon shading
#ifdef __DEPRECATED__
    double xincr = 5.;
    if ( sunShade <= 0.10 )
    {
        xincr = 25.;
    }
    else if ( sunShade <= 0.50 )
    {
        xincr = 19.;
    }
    else if ( sunShade <= 0.90 )
    {
        xincr = 12.;
    }
    else
    {
        xincr = 5.;
    }
#endif
    // This could be approximated by xinc = 25. - sunShade * 20.;
    double xincr = 25. - 20. * sunShade;
    return( airTemp + xincr );
}

//------------------------------------------------------------------------------
/*! \brief Calculates tree bark thickness using the old BEHAVE equations.
 *
 *  \param speciesIndex Index into the BehaveBarkSpecies[] array.
 *  This vakue is returned by a call to FBL_BehaveBarkSpeciesIndex().
 *  \param dbh Tree diameter at breast height (in).
 *
 *  \return Tree bark thickness (in).
 */

static double BehaveBark[][2] =
{
    { 0.000, 0.0665 },  // 0 Douglas-fir (BEHAVE code 1)
    { 0.000, 0.0650 },  // 1 western larch (BEHAVE code 1)
    { 0.056, 0.0430 },  // 2 western hemlock (BEHAVE code 2)
    { 0.189, 0.0220 },  // 3 Engelmann spruce (BEHAVE code 3)
    { 0.189, 0.0220 },  // 4 western red cedar (BEHAVE code 3)
    { 0.000, 0.0150 },  // 5 subalpine fir (BEHAVE code 4)
    { 0.000, 0.0150 }   // 6 lodgepole pine (BEHAVE code 4)
} ;

double FBL_TreeBarkThicknessBehave( int speciesIndex, double dbh )
{
    return( ( BehaveBark[ speciesIndex ][0]
            + BehaveBark[ speciesIndex ][1] * 2.54 * dbh ) / 2.54 );
}

//------------------------------------------------------------------------------
/*! \brief Calculates tree bark thickness using the FOFEM 5.0 equations.
 *
 *  \param speciesIndex Index into the FofemSpecies[] array.  This value is
 *  returned by a call to FBL_FofemSpeciesIndex().
 *  \param dbh Tree diameter at breast height (in).
 *
 *  \return Tree bark thickness (in).
 */

// Fofem factors for determining Single Bark Thickness.
// Each FOFEM species has a SBT equation index "barkEq" [1-39] into this array.
static double Fofem_Sbt[] =
{
    /* 00 */    0.000,      // Not used
    /* 01 */    0.019,      // Not used
    /* 02 */    0.022,
    /* 03 */    0.024,
    /* 04 */    0.025,
    /* 05 */    0.026,
    /* 06 */    0.027,
    /* 07 */    0.028,
    /* 08 */    0.029,
    /* 09 */    0.030,
    /* 10 */    0.031,
    /* 11 */    0.032,
    /* 12 */    0.033,
    /* 13 */    0.034,
    /* 14 */    0.035,
    /* 15 */    0.036,
    /* 16 */    0.037,
    /* 17 */    0.038,
    /* 18 */    0.039,
    /* 19 */    0.040,
    /* 20 */    0.041,
    /* 21 */    0.042,
    /* 22 */    0.043,
    /* 23 */    0.044,
    /* 24 */    0.045,
    /* 25 */    0.046,
    /* 26 */    0.047,
    /* 27 */    0.048,
    /* 28 */    0.049,
    /* 29 */    0.050,
    /* 30 */    0.052,
    /* 31 */    0.055,
    /* 32 */    0.057,      // Not used
    /* 33 */    0.059,
    /* 34 */    0.060,
    /* 35 */    0.062,
    /* 36 */    0.063,		// Changed from 0.065 to 0.063 in Build 606
    /* 37 */    0.068,
    /* 38 */    0.072,
    /* 39 */    0.081,
	/* 40 */    0.000,		// Reserved for Pinus plustris (longleaf pine)
};

double FBL_TreeBarkThicknessFofem( int speciesIndex, double dbh )
{
	// In FOFEM 6, longleaf pine (speciesIndex 122) has its own bark thickness formula
	if ( speciesIndex == 122 )
	{
		return ( 0.435 + ( 0.031 * 2.54 * dbh  ) ) / 2.54;
	}
    return( Fofem_Sbt[ FofemSpecies[ speciesIndex ].barkEq ] * dbh );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the tree crown base height.
 *
 *  \param crownRatio Tree crown ratio (crown length / tree height).
 *  \param treeHt     Tree height (ft ).
 *
 *  \return Tree's crown base height (ft).
 */

double FBL_TreeCrownBaseHeight( double crownRatio, double treeHt )
{
    return( treeHt * ( 1. - crownRatio ) );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the tree crown ratio.
 *
 *  \param baseHt   Tree crown base height (ft)
 *  \param treeHt   Tree total height (ft ).
 *
 *  \return Tree's crown ratio (fraction).
 */

double FBL_TreeCrownRatio( double baseHt, double treeHt )
{
    if ( treeHt < SMIDGEN || baseHt < 0. )
    {
        return ( 0. );
    }
    return( ( treeHt - baseHt ) / treeHt );
}

//------------------------------------------------------------------------------
/*! \brief Calculates tree crown volume scorched, tree length scorched,
 *  and tree crown length fraction scorched.
 *
 *  \param treeHt Tree height (ft).
 *  \param crownRatio Tree crown ratio (ft/ft).
 *  \param scorchHt Crown scorch height (ft).
 *  \param crownLengthScorched Address where the calculated crown length (ft)
 *  is stored.  If NULL or 0, no value is returned.
 *  \param crownLengthFractionScorched Address where the calculated crown
 *  length fraction (ft/ft) is stored.  If NULL or 0, no value is returned.
 *
 *  \return Fraction of the crown volume that is scorched (ft3/ft3).
 */

double FBL_TreeCrownScorch( double treeHt, double crownRatio, double scorchHt,
    double *crownLengthScorched, double *crownLengthFractionScorched )
{
    double scorchFrac, scorchVol;
    // Tree crown length (ft) and base height (ft)
    double crownLeng = treeHt * crownRatio;
    double baseHt    = treeHt - crownLeng;
    // Tree crown length scorched (ft)
    double scorchLeng = ( scorchHt <= baseHt )
                        ? ( 0.0 )
                        : ( scorchHt - baseHt );
    scorchLeng = ( scorchLeng > crownLeng )
                 ? ( crownLeng )
                 : ( scorchLeng );
    // Fraction of the crown length scorched (ft/ft)
    if ( crownLeng < SMIDGEN )
    {
        scorchFrac = ( scorchLeng > 0. )
                     ? ( 1.0 )
                     : ( 0.0 );
    }
    else
    {
        scorchFrac = scorchLeng / crownLeng;
    }
    // Fraction of the crown volume scorched (ft3/ft3)
    scorchVol = ( crownLeng < SMIDGEN )
                ? ( 0.0 )
                : ( scorchLeng * ( 2. * crownLeng - scorchLeng )
                    / ( crownLeng * crownLeng ) );
    // Store results in parameter addresses
    if ( crownLengthScorched )
    {
        *crownLengthScorched = scorchLeng;
    }
    if ( crownLengthFractionScorched )
    {
        *crownLengthFractionScorched = scorchFrac;
    }
    return( scorchVol );
}

//------------------------------------------------------------------------------
/*! \brief Calculates probability of tree mortality using the BEHAVE equations.
 *
 *  \param barkThickness Tree bark thickness (in).
 *  \param scorchHt Fire scorch height (ft).
 *  \param crownVolScorched Fraction of the crown volume that is scorched
 *  (ft3/ft3).
 *
 *  Note that if scorch height is zero, mortality is zero.  But once scorch
 *  height exceeds 0.0001 ft, some fire induced mortality will result as
 *  determined by bark thickness and crown volume scorched.
 *
 *  \return Tree mortality probability [0..1].
 */

double FBL_TreeMortalityBehave( double barkThickness, double scorchHt,
    double crownVolScorched )
{
    double mr = 0.;
    if ( scorchHt > 0.0001 )
    {
        // Compute the bark constant bk
        double bk = 1.466 - 4.862 * barkThickness
                  + 1.156 * barkThickness * barkThickness;
        // Determine mortality rate
        mr = 1. / (1. + exp( -( bk + 5.35 * crownVolScorched * crownVolScorched ) ) );
        // Constrain the result to the range [0..1]
        mr = ( mr < 0. )
             ? ( 0. )
             : ( mr );
        mr = ( mr > 1. )
             ? ( 1. )
             : ( mr );
    }
    return( mr );
}

//------------------------------------------------------------------------------
/*! \brief Calculates probability of tree mortality using the FOFEM 5.0
 *  equations for trees with dbh >= 1.
 *
 *  This is only a partial implementation of the FOFEM mortality algorithm.
 *  Specifically, it only implements those cases where the tree dbh >= 1".
 *  It also excludes the FOFEM special case of \e Populus \e tremuloides,
 *  which requires additional inputs (namely, flame height and fire severity).
 *
 *  \param speciesIndex Index into the FofemSpecies[] array.  This value is
 *  returned by a call to FBL_FofemSpeciesIndex().
 *  \param barkThickness Tree bark thickness (in).
 *  \param crownVolScorched Fraction of the crown volume that is scorched
 *  (ft3/ft3).
 *  \param scorchHt Scorch ht (ft)
 *
 *  \return Tree mortality probability [0..1].
 */

double FBL_TreeMortalityFofem( int speciesIndex, double barkThickness,
    double crownVolScorched, double scorchHt )
{
	double mr = 0.0;
    // Pat requested that if scorch ht is zero, then mortality is zero
    if ( scorchHt < 0.0001 )
    {
        return( mr );
    }

	// FOFEM 6 has a separate mortality equation for longleaf pine (speciesIndex == 122)
	if ( speciesIndex == 122 )
	{
		// Convert CVS from fraction to scale 1-10
		double cvs = 10. * crownVolScorched;
		if ( cvs > 0.0001 )
		{
			double bt = 2.54 * barkThickness;
			mr = 0.169
			   + ( 5.136 * bt )
			+ ( 14.492 * bt * bt )
			- ( 0.348 * cvs * cvs );
		}
	}
	// All other species use FOFEM equation 1 for dbh > 1"
	else
	{
		mr = -1.941
           + 6.316 * ( 1.0 - exp( -barkThickness ) )
           - 5.35 * crownVolScorched * crownVolScorched;
	}
    mr = 1.0 / ( 1.0 + exp( mr ) );
    // Apply minimum mortality probability for Picea (FOFEM equation 3).
    if ( FofemSpecies[speciesIndex].mortEq == 3 )
    {
		mr = ( mr < 0.8 ) ? 0.8 : mr;
    }
	// Apply minimum mortality probability for longleaf
	else if ( speciesIndex == 122 )
	{
		mr = ( mr < 0.3 ) ? 0.3 : mr;
	}
    // Confine results to range [0..1].
	mr = ( mr > 1.0 ) ? 1.0 : mr;
	mr = ( mr < 0.0 ) ? 0.0 : mr;
    return( mr );
}

//------------------------------------------------------------------------------
/*! \brief Calculates probability of tree mortality using the Hood 2008 equations.
 *
 *  \param speciesIndex Index into the HoodSpecies[] array [0..9].
 *  \param dbh Tree diameter-at-breast-height (cm).
 *  \param crownLengScorched Fraction of the crown length that is scorched (ft2/ft2).
 *  \param crownVolScorched Fraction of the crown volume that is scorched
 *  (ft3/ft3).
 *
 *  \return Tree mortality probability [0..1].
 */

double FBL_TreeMortalityHood( int speciesIndex, double dbh,
    double crownLengScorched, double crownVolScorched )
{
    double mr  = 0.00;
    // Convert to percents
    double cls = 100. * crownLengScorched;
    double cvs = 100. * crownVolScorched;
	bool zero = false;
    // White fir: crown LENGTH scorched only
	// This is Hood Eq 0, FOFEM v5 Eq 4, and FOFEM v6 Eq 10
	if ( speciesIndex == 0 )
    {
        mr = -3.5083
           + ( 0.0956 * cls )
           - ( 0.00184 * cls * cls )
           + ( 0.000017 * cls * cls * cls );
		zero = ( cls < SMIDGEN );
    }
    // Subaline and grand fir: crown VOLUME scorched only
	// This is Hood Eq 1, FOFEM v5 Eq 8, and FOFEM v6 Eq 11
    else if ( speciesIndex == 1 )
    {
        mr = -1.6950
           + ( 0.2071 * cvs )
           - ( 0.0047 * cvs * cvs )
           + ( 0.000035 * cvs * cvs * cvs );
  		zero = ( cvs < SMIDGEN );
	}
    // Red fir: crown LENGTH scorched only
	// This is Hood Eq 2, FOFEM v5 Eq 5, and FOFEM v6 Eq 16
    else if ( speciesIndex == 2 )
    {
        mr = -2.3085 + 0.000004059 * cls * cls * cls;
 		zero = ( cls < SMIDGEN );
	}
    // Incense cedar: crown LENGTH scorched only
	// This is Hood Eq 3, FOFEM v5 Eq 6, and FOFEM v6 Eq 12
    else if ( speciesIndex == 3 )
    {
        mr = -4.2466 + 0.000007172 * cls * cls * cls;
 		zero = ( cls < SMIDGEN );
	}
    // Western larch: crown VOLUME scorched and DBH
	// This is Hood Eq 4, FOFEM v5 Eq 12, and FOFEM v6 Eq 14
    else if ( speciesIndex == 4 )
    {
        mr = -1.6594
           + ( 0.0327 * cvs )
           - ( 0.0489 * dbh );
 		zero = ( cvs < SMIDGEN );
	}
    // Whitebark and lodgepole pine: crown VOLUME scorched and DBH
	// This is Hood Eq 5, FOFEM v5 Eq 13, and FOFEM v6 Eq 17
    else if ( speciesIndex == 5 )
    {
        mr = -0.3268
           + ( 0.1387 * cvs )
           - ( 0.0033 * cvs * cvs )
           + ( 0.000025 * cvs * cvs * cvs )
           - ( 0.0266 * dbh );
 		zero = ( cvs < SMIDGEN );
	}
    // Engelmann spruce: crown VOLUME scorch only
    // This is Hood Eq 6, FOFEM v5 Eq 9, and FOFEM v6 Eq 15
	else if ( speciesIndex == 6 )
    {
        mr = 0.0845 + ( 0.0445 * cvs );
		zero = ( cvs < SMIDGEN );
    }
    // Sugar pine: crown LENGTH scorch only
	// This is Hood Eq 7, FOFEM v5 Eq 7, and FOFEM v6 Eq 18
    else if ( speciesIndex == 7 )
    {
        mr = -2.0588 + ( 0.000814 * cls * cls );
		zero = ( cls < SMIDGEN );
    }
    // Ponderosa and Jeffrey pine: crown VOLUME scorch only
	// This is Hood Eq 8, FOFEM v5 Eq 10, and FOFEM v6 Eq 19
    else if ( speciesIndex == 8 )
    {
        mr = -2.7103 + ( 0.000004093 * cvs * cvs * cvs );
  		zero = ( cvs < SMIDGEN );
    }
    // Douglas-fir: crown VOLUME scorched
	// This is Hood Eq 9, FOFEM v5 Eq 11, and FOFEM v6 Eq 20
    else if ( speciesIndex == 9 )
    {
        mr = -2.0346
           + ( 0.0906 * cvs )
           - ( 0.0022 * cvs * cvs )
           + ( 0.000019 * cvs * cvs * cvs );
  		zero = ( cvs < SMIDGEN );
	}
    // UNKNOWN SPECIES INDEX
    else
    {
        fprintf( stderr, "FBL_TreeMortalityHood(): speciesIndex %d is outside valid range 0..9.",
            speciesIndex );
        return( 0.0 );
    }
	if ( zero )
	{
		return 0.;
	}
    // All Hood's equations use this form so we do them all here:
    mr = 1. / ( 1. + exp( -mr ) );
    // Confine results to range [0..1].
	mr = ( mr > 1.0 ) ? 1. : mr;
	mr = ( mr < 0.0 ) ? 0. : mr;
    return( mr );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the wind adjustment factor for scaling wind speed from
 * 20-ft to midflame height.
 *
 *  For sheltered conditions under a canopy, Albini and Baughman (1979)
 *  equation 21 (page 9) is used for the wind adjustment factor.
 *
 *  For unsheltered conditions, wind adjustment factor is calculated as an
 *	average from the top of the fuel bed to twice the fuel bed depth,
 *	using Albini and Baughman (1979) equation 9 (page 5).
 *
 *  \param canopyCover  Canopy cover projected onto ground [0..1].
 *  \param canopyHt     Tree canopy height from the ground (ft).
 *  \param crownRatio   Tree crown length-to-tree height ratio [0..1].
 *  \param fuelDepth    Fuel bed depth (ft).
 *  \param[out] fraction Pointer to a real that on return holds the fraction
 *                      of crown volume filled with tree crowns
 *  \param[out] method  Pointer to an integer that on return holds the method
 *                      used: 0=unsheltered, 1=sheltered
 *
 *  \return Wind adjustment factor [0..1].
 */

double FBL_WindAdjustmentFactor( double canopyCover, double canopyHt,
        double crownRatio, double fuelDepth, double *fraction, int *method )
{
    double waf  = 1.0;
    crownRatio  = ( crownRatio < 0.0 )  ? 0.0 : crownRatio;
    crownRatio  = ( crownRatio > 1.0 )  ? 1.0 : crownRatio;
    canopyCover = ( canopyCover < 0.0 ) ? 0.0 : canopyCover;
    canopyCover = ( canopyCover > 1.0 ) ? 1.0 : canopyCover;
    // Pat asked to delete this line on 11/9/07
    //canopyHt    = ( canopyHt < 10.0 ) ? 10.0 : canopyHt;

    // f == fraction of the volume under the canopy top that is filled with
    // tree crowns (division by 3 assumes conical crown shapes).
    double f = crownRatio * canopyCover / 3.;

    // Unsheltered
    // The following line was modified by Pat Andrews, 11/9/07
    //if ( canopyCover < SMIDGEN || f < 0.05 )
    int mthd = 0;
    if ( canopyCover < SMIDGEN || f < 0.05 || canopyHt < 6.0 )
    {
        if ( fuelDepth > SMIDGEN )
        {
            waf = 1.83 / log( (20. + 0.36 * fuelDepth) / (0.13 * fuelDepth) );
        }
        mthd = 0;
    }
    // Sheltered
    else
    {
        waf = 0.555 / ( sqrt( f * canopyHt ) * log( ( 20 + 0.36 * canopyHt )
            / ( 0.13 * canopyHt ) ) );
        mthd = 1;
    }
    // Constrain the result
    waf = ( waf > 1.0 ) ? 1.0 : waf;
    waf = ( waf < 0.0 ) ? 0.0 : waf;
    *fraction = f;
    *method = mthd;
    return( waf );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the wind chill temperature.
 *
 *  This uses the most recently (Nov 1, 2001) adopted formula
 *  used by the US NOAA and Canadian MSC and is now part of AWIPS.
 *  A new version in 2002 may add solar radiation effects.
 *
 *  \param airTemperature   Air temperature (oF).
 *  \param windSpeed        Wind speed (mi/h).
 *
 *  \return Wind chill temperature (oF).
 */

double FBL_WindChillTemperature( double airTemperature, double windSpeed )
{
    double v = 0.;
    if ( windSpeed > 0.0 )
    {
        v = pow( windSpeed, 0.16 );
    }
    double t = airTemperature;
    return( 35.74 + 0.6215 * t - 35.75 * v + 0.4275 * t * v );
    // Old method
    //return( 0.0817 * ( 5.81 + 3.71 * pow( windSpeed, 0.5 ) - 0.25 * windSpeed )
    //    * ( airTemperature - 91.4 ) + 91.4 );
}

//------------------------------------------------------------------------------
/*! \brief Calculates the wind speed at 20 ft from the wind speed at 10 m.
 *
 *  \param windSpeedAt10M Wind speed at 10 meter (mi/h).
 *
 *  \return Wind speed at 20 ft (mi/h).
 */

double FBL_WindSpeedAt20Ft( double windSpeedAt10M )
{
    return( windSpeedAt10M / 1.15 );
}

//------------------------------------------------------------------------------
//  End of xfblib.cpp
//------------------------------------------------------------------------------

