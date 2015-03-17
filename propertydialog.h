//------------------------------------------------------------------------------
/*! \file propertydialog.h
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief PropertyDialog class declarations.
 */

#ifndef _PROPERTYDIALOG_H_
/*! \def _PROPERTYDIALOG_H_
    \brief Prevent redundant includes.
 */
#define _PROPERTYDIALOG_H_ 1

// Class references
#include "appdialog.h"
#include <qptrlist.h>
class PropertyDict;
class PropertyEdit;
class PropertyPage;
class PropertyTabDialog;
class QButtonGroup;
class QComboBox;
class QGridLayout;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class VarCheckBox;

/*! \enum  PropertyWidgetType
 *  \brief Identifies the type of input widget for a property.
 */
enum PropertyWidgetType
{
    PropertyLabel=0,
    PropertyCheck=1,
    PropertyCombo=2,
    PropertyEntry=3,
    PropertyRadio=4,
    PropertySpin =5,
    PropertyGroup=6
};

//------------------------------------------------------------------------------
/*! \class PropertyEdit propertydialog.h
 *
 *  \brief PropertyEdit defines a property and its edit widget.
 */

class PropertyEdit
{
// Public methods
public:
    PropertyEdit( const QString &propName, QWidget *widget,
        PropertyWidgetType type, const QString *valueList=0, double scale=1.0 ) ;

// Public data members
public:
    QString            m_propName;  //!< Storage property name
    QWidget           *m_widget;    //!< Pointer to the property's edit widget
    PropertyWidgetType m_type;      //!< Property's edit widget type
    const QString     *m_values;    //!< Pointer to QComboBox property values
    double             m_scale;     //!< PropertySpin scale value
};

//------------------------------------------------------------------------------
/*! \class PropertyPage propertydialog.h
 *
 *  \brief PropertyPage defines a single tab page in the PropertyDialog.
 */

class PropertyPage : public AppPage
{
// Public methods
public:
    PropertyPage( PropertyTabDialog *dialog, int rows, int cols,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile, const char *name=0 ) ;
    ~PropertyPage( void ) ;

    QPushButton  *addButton( const QString &labelKey,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align=0 ) ;

    QButtonGroup *addButtonGroup( const QString &labelKey,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align=0 ) ;

    VarCheckBox  *addCheck( const QString &propName,
        const QString &labelKey, const QString &htmlFile,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align=0 ) ;

    QComboBox    *addCombo( const QString &propName,
        const QString keyList[], const QString valueList[], int listSize,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align=0 ) ;

    QLineEdit    *addEntry( const QString &propName,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align=0 ) ;

    QLabel       *addLabel( const QString &labelKey,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align=AlignLeft ) ;

    QRadioButton *addRadio( const QString &propName, const QString &labelKey,
        QButtonGroup *group ) ;

    QRadioButton *addRadio( const QString &propName, const QString &labelKey,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align=0 ) ;

    QSpinBox     *addSpin( const QString &propName,
        int minVal, int maxVal, int stepVal,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align=0,
        double scale=1.0 ) ;

// Public data members
public:
    PropertyTabDialog   *m_dialog;  //!< Pointer to parent PropertyDialog
    QFrame              *m_frame;   //!< Frame to hold the QGridLayout.
    QGridLayout         *m_grid;    //!< Pointer to the grid layout widget
    QPtrList<PropertyEdit> *m_editList;//<! List of pointers to input widgets
};

//------------------------------------------------------------------------------
/*! \class PropertyTabDialog propertydialog.h
 *
 *  \brief PropertyTabDialog provides the user with an input interface for
 *  setting property values.
 *  It inherits AppTabDialog to display one or more AppPages.
 *  Each page has a GridLayout in which various types of input widgets are added.
 *  The input widgets' initial settings are read from a PropertyDict, and
 *  the input widgets' final values are stored back into the PropertyDict.
 */

class PropertyTabDialog : public AppTabDialog
{
// Enable signals and slots
    Q_OBJECT

// Public methods
public:
    PropertyTabDialog( QWidget *parent, PropertyDict *prop,
        const QString &captionKey="UnitsEditor:Caption",
        const char *name="propertyTabDialog",
        const QString &acceptKey="PropertyTabDialog:Button:Accept",
        const QString &rejectKey="PropertyTabDialog:Button:Reject" ) ;
    ~PropertyTabDialog( void ) ;
    virtual PropertyPage *addPage( const QString &tabKey, int rows, int cols,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile ) ;

// Protect slots
protected slots:
	virtual void tabChanged( QWidget *tab );
    virtual void store( void );

// Public data members
public:
    PropertyDict        *m_prop;    //!< Pointer to SHARED PropertyDict
    QPtrList<PropertyPage> *m_pageList;//!< List of pointers to child PropertyPages
};

#endif

//------------------------------------------------------------------------------
//  End of propertydialog.h
//------------------------------------------------------------------------------

