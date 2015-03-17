//------------------------------------------------------------------------------
/*! \file filesystem.h
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2013 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief FileSystem class declaration.
 */

#ifndef _FILESYSTEM_H_
/*! \def _FILESYSTEM_H_
 *  \brief Prevent redundant includes.
 */
#define _FILESYSTEM_H_ 1

// Custom classes
#include "xeqfile.h"

// Qt class references
#include <qptrlist.h>
#include <qobject.h>
#include <qstring.h>

//------------------------------------------------------------------------------
/*! \class FileSystem filesystem.h
 *
 *  \brief The FileSystem class contains all the BehavePlus directory and file
 *  and file structure definition.
 *
 *  Any method ending in *Path() returns a full directory and/or file path name.
 *  Any method ending in *Folder() returns just the directory base name.
 *  Any method ending in *File() returns just the file name as "basename.ext".
 *  Any method ending in *Ext() returns just the extension name (without period).
 */

class FileSystem : public QObject
{
public:
    FileSystem( void );

    // The following methods get/set directory and file names
    QString workspacePath( const QString &newDataPath="" ) ;
    QString installPath( const QString &newInstallPath="" ) ;
    QString language( const QString &newLanguage="" ) ;

    // The following methods get a full directory and/or a full file path name
    QString capturePath( const QString &fileName="" ) ;
    QString composerPath( const QString &fileName="" ) ;
    QString defaultWorkspacePath( const QString &fileName="" ) ;
    QString docGeoPath( const QString &fileName="" ) ;
    QString docHtmlPath( const QString &fileName="" ) ;
    QString exportPath( const QString &fileName="" ) ;
    QString fuelModelPath( const QString &fileName="" ) ;
    QString imagePath( const QString &fileName="" ) ;
    QString moisScenarioPath( const QString &fileName="" ) ;
    QString runPath( const QString &fileName="" ) ;
    QString unitsSetPath( const QString &fileName="" ) ;
    QString worksheetPath( const QString &fileName="" ) ;

    // The following methods get specific file full path names
    QString composerFilePath( int docId, int pageNo ) ;
    QString docHtmlIndexFilePath( void ) ;
    QString helpFilePath( void ) ;
    QString featuresPaperFilePath( void ) ;
    QString propertyFilePath( void ) ;
    QString tempFilePath( int docId ) ;
    QString userGuideFilePath( void ) ;
    QString variableRefFilePath( void ) ;
    QString xmlFilePath( void ) ;

    // The following methods get a specified base file name
    QString docHtmlIndexFile( void ) const ;
    QString featuresPaperFile( void ) ;
    QString helpFile( void ) ;
    QString unNamedFile( void ) const ;
    QString userGuideFile( void ) ;
    QString variableRefFile( void ) ;
    QString xmlFile( void ) ;

    // The following methods get standard file extensions
    QString fuelModelExt( void ) const ;
    QString moisScenarioExt( void ) const ;
    QString runExt( void ) const ;
    QString unitsSetExt( void ) const ;
    QString worksheetExt( void ) const ;
    QString type( const QString &fileName ) const ;

    // The following methods get a list of files that match a pattern
    int  findFiles( const QString &pattern, const QString &extension,
            const QString &folder, bool fullPath, bool getDescriptions,
            QStringList *fileList ) ;
    int  findFuelModelFiles( const QString &pattern, bool fullPath,
            bool getDescriptions, QStringList *fileList ) ;
    int  findMoisScenarioFiles( const QString &pattern, bool fullPath,
            bool getDescriptions, QStringList *fileList ) ;
    int  findRunFiles( const QString &pattern, bool fullPath,
            bool getDescriptions, QStringList *fileList ) ;
    int  findUnitsSetFiles( const QString &pattern, bool fullPath,
            bool getDescriptions, QStringList *fileList ) ;
    int  findWorksheetFiles( const QString &pattern, bool fullPath,
            bool getDescriptions, QStringList *fileList ) ;

    // The following methods test the installation
    bool findInstallationDir( QString &errMsg ) ;
    bool findInstallationDir2( const QString &label, const QString &dir,
            QString &errMsg ) ;
    bool findInstallationStartupFiles( const QString &dirName, QString &errMsg ) ;
    bool geoFileExists( const QString &fileName, QString &errMsg ) ;
    bool htmlFileExists( const QString &fileName, QString &errMsg ) ;
    bool imageFileExists( const QString &fileName, QString &errMsg ) ;
    bool isValidDirectory( const QString &dirName, QString &errMsg ) ;
    bool isValidInstallation( const QString &installDir, QString &errMsg ) ;
    void setFileList( QPtrList<EqFile> *fileList ) ;
    bool testGeoFiles( QString &errMsg ) ;
    bool testHtmlFiles( QString &errMsg ) ;
    bool testImageFiles( QString &errMsg ) ;
    bool testWorkspaceFiles( const QString &workspacePath, QString &errMsg ) ;
    QString useDefaultWorkspace( void ) ;

    // The following methods create/delete folders and files
    bool makeWorkspace( const QString &newWorkspacePath,
            const QString &srcWorkspacePath, bool clone, QString &errMsg ) ;

    // I/O
    void printHtmlFiles( QWidget* parent ) ;

// Private data members
private:
    // Path locations that may vary
    QString m_installPath;          //!< Installation home directory full path name
    QString m_workspacePath;        //!< Current home data directory full path name
    QString m_separator;            //!< System-specific path name separator ("/" or "\")
    QString m_language;             //!< Current documentation language directory base name ("en_US")
    QString m_captureFolder;        //!< Capture directory base name ("CaptureFolder")
    QString m_capturePath;          //!< Capture directory full path name
    QString m_cloneFolder;          //!< Clone directory base name ("WorksheetFolder")
    QString m_clonePath;            //!< Clone directory full path name
    QString m_composerFile;         //!< Most recent Composer file full path name as constructed by composerFilePath()
    QString m_composerFolder;       //!< Composer directory base name ("ComposerFolder")
    QString m_composerPath;         //!< Composer directory full path name
    QString m_defaultWorkspaceFolder; //!< Default data folder directory base name ("DefaultDataFolder")
    QString m_defaultWorkspacePath; //!< Default data folder full path name
    QString m_docGeoFolder;         //!< Geographic data directory base name ("GeoFolder")
    QString m_docGeoPath;           //!< Geographic data directory full path name
    QString m_docFolder;            //!< Documentation directory base name ("DocFolder")
    QString m_docHtmlIndexFile;     //!< HTML index file name ("index.html")
    QString m_docHtmlIndexFilePath; //!< HTML index file full path name
    QString m_docHtmlFolder;        //!< HTML documentation directory base name ("Html")
    QString m_docHtmlPath;          //!< HTML documentation directory full path name
    QString m_exportFolder;         //!< Export directory base name ("WorksheetFolder")
    QString m_exportPath;           //!< Export directory full path name
    QString m_featuresPaperFile;    //!< Features paper file name (FeaturesPaper.pdf)
    QString m_featuresPaperFilePath;//!< Features paper file full path name
    QString m_fuelModelFolder;      //!< Fuel model directory base name ("FuelModelFolder")
    QString m_fuelModelPath;        //!< Fuel model directory full path name
    QString m_helpFile;             //!< Help file name ("BehavePlus.hlp")
    QString m_helpFilePath;         //!< Help file full path name
    QString m_imageFolder;          //!< Image directory base name ("ImageFolder")
    QString m_imagePath;            //!< Image directory full path name
    QString m_moisScenarioFolder;   //!< Moisture scenario directory base name ("MoistureScenarioFolder")
    QString m_moisScenarioPath;     //!< Moisture scenario directory full path name
    QString m_propertyFile;         //!< Application property file name ("property.dat")
    QString m_propertyFilePath;     //!< Application property file full path name
    QString m_runFolder;            //!< Run directory base name ("RunFolder")
    QString m_runPath;              //!< Run directory full path name
    QString m_splashFile;           //!< Splash image file ("Wildfire.png")
    QString m_unitsSetFolder;       //!< Units set directory base name ("UnitsSetFolder")
    QString m_unitsSetPath;         //!< Units set directory full path name
    QString m_unNamedFile;          //!< Unnamed file base name ("unnamed")
    QString m_userGuideFile;        //!< User guide file name ("UsersGuide.pdf")
    QString m_userGuideFilePath;    //!< User guide file full path name
    QString m_variableRefFile;      //!< Variable reference file name ("VariableRef.pdf")
    QString m_variableRefFilePath;  //!< Variable reference file full path name
    QString m_worksheetFolder;      //!< Worksheet directory base name ("WorksheetFolder")
    QString m_worksheetPath;        //!< Worksheet directory full path name
    QString m_fuelModelExt;         //!< Fuel model file extension ("bpf")
    QString m_moisScenarioExt;      //!< Moisture scenario file extensions ("bpm")
    QString m_runExt;               //!< Run file extensions ("bpr")
    QString m_unitsSetExt;          //!< Units set file extension ("bpu")
    QString m_worksheetExt;         //!< Worksheet file extension ("bpw")
    QString m_xmlFile;              //!< BehavePlus XML definition file ("BehavePlus3.xml")
    QString m_xmlFilePath;          //!< BehavePlus XML definition file path ("BehavePlus3.xml")
    QPtrList<EqFile> *m_fileList;   //!< List of EqFiles
};

// Non-member support methods.
bool addExtension( QString &fileName, const QString &ext ) ;

int  fileDescription( const char *fileName, QString &desc ) ;

int  folderDescription( QString &fileName, QString &desc ) ;

bool isBehavePlusXmlDocument( const QString &fileName ) ;

void makeFileTable( QStringList &list, const QString &title,
        QString &table ) ;

void replaceXmlEntities( QString &text ) ;

bool subdirCopy( const QString &srcPath, const QString &dstPath,
        QString &errMsg ) ;

bool textFileCopy( const QString &srcPath, const QString &dstPath,
        QString &errMsg ) ;

#endif

//------------------------------------------------------------------------------
//  End of filesystem.h
//------------------------------------------------------------------------------

