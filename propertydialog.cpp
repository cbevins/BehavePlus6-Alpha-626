//------------------------------------------------------------------------------
/*! \file propertydialog.cpp
 *  \version BehavePlus3
 *  \author Copyright (C) 2002-2004 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief PropertyDialog class declarations.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "propertydialog.h"
#include "property.h"
#include "varcheckbox.h"

// Qt include files
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qwidget.h>

// Standard include files
#include <math.h>

//------------------------------------------------------------------------------
/*! \brief PropertyEdit constructor.
 *
 *  \param propName Storage property name
 *  \param widget   Pointer to the input edit widget.
 *  \param type     One of PropertyLabel, PropertyCheck, PropertyCombo,
 *                  PropertyRadio, or PropertySpin.
 *  \param values   Optional pointer to property value array (used only by
 *                  the QComboBox).
 *  \param scale    Factor used only by QSpinBox to scale the property from its
 *                  native real units to QSpinBox integer text.
 *
 *  \note This is normally called only by the convenience routines
 *  PropertyPage::addCheck(), PropertyPage::addCombo(), PropertyPage::addEntry(),
 *  PropertyPage::addRadio(), and PropertyPage::addSpin().
 */

PropertyEdit::PropertyEdit( const QString &propName, QWidget *widget,
        PropertyWidgetType type, const QString *values, double scale ) :
    m_propName(propName),
    m_widget(widget),
    m_type(type),
    m_values(values),
    m_scale(scale)
{
    if ( m_type == PropertySpin
      && scale < 0.00001 )
    // This code block should never be executed!
    {
        QString text("");
        bomb( QString( "PropertyEdit::PropertyEdit() - "
            "Real Property \"%1\" has a scale factor near zero (%2)." )
            .arg( propName ).arg( scale ) );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief PropertyPage constructor.
 *
 *  \param dialog       Pointer to parent PropertyTabDialog
 *  \param rows         Number of rows on the page
 *  \param cols         Number of columns on the page
 *  \param pictureFile  Picture file name
 *  \param pictureName  Picture name
 *  \param htmlFile     Page's initial HTML file base name
 *  \param name         Widget's internal name
 */

PropertyPage::PropertyPage( PropertyTabDialog *dialog, int rows, int cols,
        const QString &pictureFile, const QString &pictureName,
        const QString &htmlFile, const char *name ) :
    AppPage( dialog, pictureFile, pictureName, htmlFile, name ),
    m_dialog(dialog),
    m_frame(0),
    m_grid(0),
    m_editList(0)
{
    // Hidden frame to contain a grid layout.
    m_frame = new QFrame( m_contentFrame, QString( "%1:Frame" ).arg( name ) );
    Q_CHECK_PTR( m_frame );
    m_frame->setFrameStyle( QFrame::NoFrame );

    // Create grid layout with 0 border and 2 spacing.
    m_grid = new QGridLayout( m_frame, rows, cols, 0, 2,
        QString( "%1:Grid" ).arg( name ) );
    Q_CHECK_PTR( m_grid );
    // Create the page's PropertyEdit list
    m_editList = new QPtrList<PropertyEdit>();
    Q_CHECK_PTR( m_editList );
    // We do want to delete the PropertyEdit's when this class is destroyed.
    m_editList->setAutoDelete( true );
    return;
}

//------------------------------------------------------------------------------
/*! \brief PropertyPage destructor.
 */

PropertyPage::~PropertyPage( void )
{
    delete m_editList;     m_editList = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds a QButton to the PropertyPage.
 *
 *  \param labelKey Button text translation key.
 *  \param rowBeg   Begining grid row
 *  \param colBeg   Begining grid column
 *  \param rowEnd   Ending grid row
 *  \param colEnd   Ending grid column
 *  \param align    Alignment
 *
 *  \return Pointer to the new QLabel widget.
 */

QPushButton *PropertyPage::addButton( const QString &labelKey,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align )
{
    // Translate the label
    QString label("");
    translate( label, labelKey );
    // Create the QLabel
    QPushButton *widget = new QPushButton( label, m_frame, labelKey );
    Q_CHECK_PTR( widget );
    // Add the widget to the grid layout
    m_grid->addMultiCellWidget( widget, rowBeg, rowEnd, colBeg, colEnd, align );
    // Return the address of the newly created QRadioButton
    return( widget );
}

//------------------------------------------------------------------------------
/*! \brief Adds a new QButtonGroup to the PropertyPage.
 *
 *  \param labelKey Label translation key and widget name.
 *  \param rowBeg   Begining grid row
 *  \param colBeg   Begining grid column
 *  \param rowEnd   Ending grid row
 *  \param colEnd   Ending grid column
 *  \param align    Alignment
 *
 *  \return Pointer to the new QButtonGroup widget.
 */

QButtonGroup *PropertyPage::addButtonGroup( const QString &labelKey,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align )
{
    // Translate the label
    QString label("");
    translate( label, labelKey );
    // Create the button group
    QButtonGroup *widget = new QButtonGroup(
        1,                                  // Number of button columns
        Qt::Horizontal,                     // Orientation
        label,                              // Translated label text
        m_frame,                            // Parent widget
        labelKey );                         // Widget internal name
    Q_CHECK_PTR( widget );
    // Add the widget to the grid layout
    m_grid->addMultiCellWidget( widget, rowBeg, rowEnd, colBeg, colEnd, align );
    return( widget );
}

//------------------------------------------------------------------------------
/*! \brief Adds a new VarCheckBox to the PropertyPage.
 *
 *  \param propName Storage property name (and internal widget name).
 *  \param labelKey Label translation key.
 *  \param htmlFile VarCheckBox HTML file name.
 *  \param rowBeg   Begining grid row
 *  \param colBeg   Begining grid column
 *  \param rowEnd   Ending grid row
 *  \param colEnd   Ending grid column
 *  \param align    Alignment
 *
 *  \return Pointer to the new VarCheckBox widget.
 */

VarCheckBox *PropertyPage::addCheck( const QString &propName,
        const QString &labelKey, const QString &htmlFile,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align )
{
    // Only display properties that are part of this release
    Property *prop = m_dialog->m_prop->find( propName );
    if ( ! prop )
    {
fprintf( stderr, "\n\nProperty not found.\n\n" );
        return( 0 );
    }
    if ( ! prop->isCurrent( appWindow()->m_release ) )
    {
fprintf( stderr, "\n\nProperty not current.\n\n" );
        return( 0 );
    }
    // Translate the label key
    QString label("");
    translate( label, labelKey );
    // Create the VarCheckBox
    VarCheckBox *widget = new VarCheckBox( label, htmlFile, m_helpBrowser,
        m_frame, propName );
    checkmem( __FILE__, __LINE__, widget, "VarCheckBox widget", 1 );
    // Initialize its state
    widget->setChecked( m_dialog->m_prop->boolean( propName ) );
    
    // Add the widget to the grid layout
    m_grid->addMultiCellWidget( widget, rowBeg, rowEnd, colBeg, colEnd, align );
    // Create the PropertyEdit
    PropertyEdit *edit = new PropertyEdit( propName, widget, PropertyCheck );
    checkmem( __FILE__, __LINE__, edit, "PropertyEdit edit", 1 );
    // Add the PropertyEdit to the PropertyPage's m_editList
    m_editList->append( edit );
    // Return the address of the newly created VarCheckBox
    return( widget );
}

//------------------------------------------------------------------------------
/*! \brief Adds a read-only QComboBox to the PropertyPage.
 *
 *  \param propName  Storage property name (and internal widget name).
 *  \param keyList   Array of combo text translation keys
 *  \param valueList Array of associated proeprty values
 *  \param listSize  Insert this many items from the list
 *  \param rowBeg    Beginning grid row
 *  \param colBeg    Beginning grid column
 *  \param rowEnd    Ending grid row
 *  \param colEnd    Ending grid column
 *  \param align     Alignment
 *
 *  \return Pointer to the new QComboBox widget.
 */

QComboBox *PropertyPage::addCombo( const QString &propName,
        const QString keyList[], const QString valueList[], int listSize,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align )
{
    // Only display properties that are part of this release
    Property *prop = m_dialog->m_prop->find( propName );
    if ( ! prop
      || ! prop->isCurrent( appWindow()->m_release ) )
    {
        return( 0 );
    }
    // Create the QComboBox
    QComboBox *widget = new QComboBox( false, m_frame, propName );
    Q_CHECK_PTR( widget );
    // Insert items into the combo box while searching for the current item
    int n = 0;
    QString currentItem = m_dialog->m_prop->value( propName );
    QString text("");
    for ( int id = 0;
          id < listSize;
          id++ )
    {
        translate( text, keyList[id] );
        widget->insertItem( text );
        if ( valueList[id] == currentItem )
        {
            n = id;
        }
    }
    // Initialize its value
    widget->setCurrentItem( n );
    // Add the widget to the grid layout
    m_grid->addMultiCellWidget( widget, rowBeg, rowEnd, colBeg, colEnd, align );
    // Create the PropertyEdit
    PropertyEdit *edit = new PropertyEdit( propName, widget, PropertyCombo,
        valueList );
    checkmem( __FILE__, __LINE__, edit, "PropertyEdit edit", 1 );
    // Add the PropertyEdit to the PropertyPage's m_editList
    m_editList->append( edit );
    // Return the address of the newly created QComboBox
    return( widget );
}

//------------------------------------------------------------------------------
/*! \brief Adds a new QLineEdit to the PropertyPage.
 *
 *  \param propName Storage property name (and internal widget name).
 *  \param rowBeg   Begining grid row
 *  \param colBeg   Begining grid column
 *  \param rowEnd   Ending grid row
 *  \param colEnd   Ending grid column
 *  \param align    Alignment
 *
 *  \return Pointer to the new QLineEdit widget.
 */

QLineEdit *PropertyPage::addEntry( const QString &propName,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align )
{
    // Only display properties that are part of this release
    Property *prop = m_dialog->m_prop->find( propName );
    if ( ! prop
      || ! prop->isCurrent( appWindow()->m_release ) )
    {
        return( 0 );
    }
    // Create the QLineEdit with the current property text
    QLineEdit *widget = new QLineEdit( m_dialog->m_prop->string( propName ),
        m_frame, propName );
    Q_CHECK_PTR( widget );
    // Add the widget to the grid layout
    m_grid->addMultiCellWidget( widget, rowBeg, rowEnd, colBeg, colEnd, align );
    // Create the PropertyEdit
    PropertyEdit *edit = new PropertyEdit( propName, widget, PropertyEntry );
    checkmem( __FILE__, __LINE__, edit, "PropertyEdit edit", 1 );
    // Add the PropertyEdit to the PropertyPage's m_editList
    m_editList->append( edit );
    // Return the address of the newly created QLineEdit
    return( widget );
}

//------------------------------------------------------------------------------
/*! \brief Adds a QLabel to the PropertyPage.
 *
 *  \param labelKey Label translation key.
 *  \param rowBeg   Begining grid row
 *  \param colBeg   Begining grid column
 *  \param rowEnd   Ending grid row
 *  \param colEnd   Ending grid column
 *  \param align    Alignment
 *
 *  \return Pointer to the new QLabel widget.
 */

QLabel *PropertyPage::addLabel( const QString &labelKey,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align )
{
    // Translate the label
    QString label("");
    translate( label, labelKey );
    // Create the QLabel
    QLabel *widget = new QLabel( label, m_frame, labelKey );
    Q_CHECK_PTR( widget );
    // Add the widget to the grid layout
    m_grid->addMultiCellWidget( widget, rowBeg, rowEnd, colBeg, colEnd, align );
    // Return the address of the newly created QRadioButton
    return( widget );
}

//------------------------------------------------------------------------------
/*! \brief Adds a QRadioButton to a QButtonGroup on the PropertyPage.
 *
 *  \param propName Storage property name (and internal widget name).
 *  \param labelKey Label translation key.
 *  \param buttonGroup Pointer to the QButtonGroup.
 *
 *  \return Pointer to the new QRadioButton widget.
 */

QRadioButton *PropertyPage::addRadio( const QString &propName,
        const QString &labelKey, QButtonGroup *buttonGroup )
{
    // Only display properties that are part of this release
    Property *prop = m_dialog->m_prop->find( propName );
    if ( ! prop
      || ! prop->isCurrent( appWindow()->m_release ) )
    {
        return( 0 );
    }
    // Translate the label
    QString label("");
    translate( label, labelKey );
    // Create the QRadioButton
    QRadioButton *widget = new QRadioButton( label, buttonGroup, propName );
    Q_CHECK_PTR( widget );
    // Initialize its state
    widget->setChecked( m_dialog->m_prop->boolean( propName ) );
    
    // Create the PropertyEdit
    PropertyEdit *edit = new PropertyEdit( propName, widget, PropertyRadio );
    checkmem( __FILE__, __LINE__, edit, "PropertyEdit edit", 1 );
    // Add the PropertyEdit to the PropertyPage's m_editList
    m_editList->append( edit );
    // Return the address of the newly created QRadioButton
    return( widget );
}

//------------------------------------------------------------------------------
/*! \brief Adds a QRadioButton to the PropertyPage.
 *
 *  \param propName Storage property name (and internal widget name).
 *  \param labelKey Label translation key.
 *  \param rowBeg   Begining grid row
 *  \param colBeg   Begining grid column
 *  \param rowEnd   Ending grid row
 *  \param colEnd   Ending grid column
 *  \param align    Alignment
 *
 *  \return Pointer to the new QRadioButton widget.
 */

QRadioButton *PropertyPage::addRadio( const QString &propName,
        const QString &labelKey,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align )
{
    // Only display properties that are part of this release
    Property *prop = m_dialog->m_prop->find( propName );
    if ( ! prop
      || ! prop->isCurrent( appWindow()->m_release ) )
    {
        return( 0 );
    }
    // Translate the label
    QString label("");
    translate( label, labelKey );
    // Create the QRadioButton
    QRadioButton *widget = new QRadioButton( label, m_frame, propName );
    Q_CHECK_PTR( widget );
    // Initialize its state
    widget->setChecked( m_dialog->m_prop->boolean( propName ) );

    // Add the widget to the grid layout
    m_grid->addMultiCellWidget( widget, rowBeg, rowEnd, colBeg, colEnd, align );
    // Create the PropertyEdit
    PropertyEdit *edit = new PropertyEdit( propName, widget, PropertyRadio );
    checkmem( __FILE__, __LINE__, edit, "PropertyEdit edit", 1 );
    // Add the PropertyEdit to the PropertyPage's m_editList
    m_editList->append( edit );
    // Return the address of the newly created QRadioButton
    return( widget );
}

//------------------------------------------------------------------------------
/*! \brief Adds a QSpinBox to the PropertyPage.
 *
 *  \param propName Storage property name (and internal widget name).
 *  \param minVal   Minimum value
 *  \param maxVal   Maximum value
 *  \param stepVal  Step size
 *  \param rowBeg   Begining grid row
 *  \param colBeg   Begining grid column
 *  \param rowEnd   Ending grid row
 *  \param colEnd   Ending grid column
 *  \param align    Alignment
 *  \param scale    Scale factor for converting the QSpinBox value from its
 *                  native value into the spin box integer value.
 *                  For example, a scale of 100. converts a fraction value
 *                  [0..1] into a QSPinBox percent entry [0..100].
 *                  NOTE: This must NOT BE ZERO!!
 *
 *  \return Pointer to the new QSpinBox widget.
 */

QSpinBox *PropertyPage::addSpin( const QString &propName,
        int minVal, int maxVal, int stepVal,
        int rowBeg, int colBeg, int rowEnd, int colEnd, int align,
        double scale )
{
    // Only display properties that are part of this release
    Property *prop = m_dialog->m_prop->find( propName );
    if ( ! prop
      || ! prop->isCurrent( appWindow()->m_release ) )
    {
        return( 0 );
    }
    // Create the QSpinBox
    QSpinBox *widget = new QSpinBox( minVal, maxVal, stepVal, m_frame,
        propName );
    Q_CHECK_PTR( widget );
    // Initialize its value
    if ( m_dialog->m_prop->type( propName ) == Property::Integer )
    {
        widget->setValue( m_dialog->m_prop->integer( propName ) );
    }
    else
    {
        widget->setValue( (int) ( scale * m_dialog->m_prop->real( propName ) ) );
    }
    // Add the widget to the grid layout
    m_grid->addMultiCellWidget( widget, rowBeg, rowEnd, colBeg, colEnd, align );
    // Create the PropertyEdit
    PropertyEdit *edit = new PropertyEdit( propName, widget, PropertySpin,
        0, scale );
    checkmem( __FILE__, __LINE__, edit, "PropertyEdit edit", 1 );
    // Add the PropertyEdit to the PropertyPage's m_editList
    m_editList->append( edit );
    // Return the address of the newly created QSpinBox
    return( widget );
}

//------------------------------------------------------------------------------
/*! \brief PropertyTabDialog constructor.
 *
 *  \param parent       Pointer to the parent widget.
 *  \param prop         Pointer to the PropertyDict.
 *  \param captionKey   Dialog caption translator key.
 *  \param name         Widget internal name.
 *  \param acceptKey    Translator key for the accept button
 *  \param rejectKey    Translator key for the reject button
 */

PropertyTabDialog::PropertyTabDialog( QWidget *parent, PropertyDict *prop,
        const QString &captionKey, const char *name,
        const QString &acceptKey, const QString &rejectKey ) :
    AppTabDialog( parent, captionKey, name, acceptKey, rejectKey ),
    m_prop(prop),
    m_pageList(0)
{
    // Create the PropertyPage list
    m_pageList = new QPtrList<PropertyPage>();
    Q_CHECK_PTR( m_pageList );
    m_pageList->setAutoDelete( false );

	// Capture all tab changes
	connect( this, SIGNAL( currentChanged( QWidget* ) ),
             this, SLOT( tabChanged( QWidget*) ) );

    return;
}

//------------------------------------------------------------------------------
/*! \brief PropertyTabDialog destructor.
 */

PropertyTabDialog::~PropertyTabDialog( void )
{
    delete m_pageList;  m_pageList = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Adds an PropertyPage to the PropertyTabDialog.
 *
 *  \param tabText      Tab translator key.
 *  \param rows         Number of rows on the page.
 *  \param cols         Number of columns on the page.
 *  \param pictureFile  Picture file name.
 *  \param pictureName  Picture name.
 *  \param htmlFile     Page's initial HTML file base name.
 *
 *  \return Pointer to the new PropertyPage.
 */

PropertyPage *PropertyTabDialog::addPage( const QString &tabKey,
        int rows, int cols, const QString &pictureFile,
        const QString &pictureName, const QString &htmlFile )
{
    // Create the new page
    PropertyPage *page = new PropertyPage( this, rows, cols, pictureFile,
        pictureName, htmlFile, QString( "%1%2" ).arg( name() ).arg( tabKey ) );
    checkmem( __FILE__, __LINE__, page, "PropertyPage page", 1 );
    // Add it to the dialog's page list
    m_pageList->append( page );
    // Add the tab
    if ( ! tabKey.isNull() && ! tabKey.isEmpty() )
    {
        QString tabText("");
        translate( tabText, tabKey );
        addTab( page, tabText );
    }
    return( page );
}

//------------------------------------------------------------------------------
/*! \brief Re-implemented method to store current button states into their
 *  associated property values.  This is the PropertyTabDialog's Accept button
 *  callback.
 */

void PropertyTabDialog::store( void )
{
    PropertyPage *page;
    PropertyEdit *edit;
    for ( page = m_pageList->first();
          page != 0;
          page = m_pageList->next() )
    {
        for ( edit = page->m_editList->first();
              edit != 0;
              edit = page->m_editList->next() )
        {
            if ( edit->m_type == PropertyRadio )
            {
                QRadioButton *w = (QRadioButton *) edit->m_widget;
                m_prop->boolean( edit->m_propName, w->isChecked() );
            }
            else if ( edit->m_type == PropertyCheck )
            {
                VarCheckBox *w = (VarCheckBox *) edit->m_widget;
                m_prop->boolean( edit->m_propName, w->isChecked() );
            }
            else if ( edit->m_type == PropertyCombo )
            {
                QComboBox *w = (QComboBox *) edit->m_widget;
                // All values from a QComboBox are presumed to be valid,
                // so we can store property via update() regardless of type.
                m_prop->update( edit->m_propName,
                    edit->m_values[ w->currentItem() ] );
            }
            else if ( edit->m_type == PropertyEntry )
            {
                QLineEdit *w = (QLineEdit *) edit->m_widget;
                m_prop->string( edit->m_propName, w->text() );
            }
            else if ( edit->m_type == PropertySpin )
            {
                // All values from a QSpinBox are presumed to be valid,
                // so we can store property via update() regardless of type.
                QSpinBox *w  = (QSpinBox *) edit->m_widget;
                //m_prop->integer( edit->m_propName, w->value() );
                QString text( w->text() );
                // Do we need to apply a scale factor?
                if ( fabs( edit->m_scale - 1.0 ) > 0.00001 )
                {
                    double value = w->value() / edit->m_scale;
                    text.sprintf( "%f", value );
                }
                m_prop->update( edit->m_propName, text );
            }
        }
    }
    return;
}
//------------------------------------------------------------------------------
/*! \brief Called whenever the user selects a new tab.
 */

void PropertyTabDialog::tabChanged( QWidget *tab )
{
}

//------------------------------------------------------------------------------
//  End of propertydialog.cpp
//------------------------------------------------------------------------------

