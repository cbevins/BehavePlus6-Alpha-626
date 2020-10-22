//------------------------------------------------------------------------------
/*! \file filesystem.cpp
 *  \version BehavePlus6
 *  \author Copyright (C) 2002-2018 by Collin D. Bevins.  All rights reserved.
 *
 *  \brief FileSystem class methods.
 */

// Custom include files
#include "appmessage.h"
#include "apptranslator.h"
#include "appwindow.h"
#include "filesystem.h"
#include "platform.h"
#include "textview.h"

// Qt include files
#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qprogressdialog.h>
#include <qtextstream.h>

// Standard include files
#include <stdlib.h>
#include <ctype.h>

// Buffer sizes
#define BIG_BUFFER      (1024)
#define SMALL_BUFFER    (256)

//------------------------------------------------------------------------------
/*! \brief FileSystem class constructor that initializes all extensions and folder
 *  names and makes the current active directory the data home directory.
 */

FileSystem::FileSystem( void ) :
    m_installPath(""),
    m_workspacePath(""),
    m_separator(""),
    m_language("en_US"),
    m_captureFolder("CaptureFolder"),
    m_capturePath(""),
    m_composerFile(""),
    m_composerFolder("ComposerFolder"),
    m_composerPath(""),
    m_defaultWorkspaceFolder("DefaultDataFolder"),
    m_defaultWorkspacePath(""),
    m_docGeoFolder("GeoFolder"),
    m_docGeoPath(""),
    m_docFolder("DocFolder"),
    m_docHtmlIndexFile("index.html"),
    m_docHtmlIndexFilePath(""),
    m_docHtmlFolder("Html"),
    m_docHtmlPath(""),
    m_exportFolder("ExportFolder"),
    m_exportPath(""),
    m_featuresPaperFile("FeaturesPaper.pdf"),
    m_featuresPaperFilePath(""),
    m_fuelModelFolder("FuelModelFolder"),
    m_fuelModelPath(""),
    m_helpFile("UsersGuide.pdf"),
    m_helpFilePath(""),
    m_imageFolder("ImageFolder"),
    m_imagePath(""),
    m_moisScenarioFolder("MoistureScenarioFolder"),
    m_moisScenarioPath(""),
    m_propertyFile("property.xml"),
    m_propertyFilePath(""),
    m_runFolder("RunFolder"),
    m_runPath(""),
    m_splashFile("Wildfire.png"),
    m_unitsSetFolder("UnitsSetFolder"),
    m_unitsSetPath(""),
    m_unNamedFile("unnamed"),
    m_userGuideFile("UsersGuide.pdf"),
    m_userGuideFilePath(""),
    m_variableRefFile("VariableRef.pdf"),
    m_variableRefFilePath(""),
    m_worksheetFolder("WorksheetFolder"),
    m_worksheetPath(""),
    m_fuelModelExt("bpf"),
    m_moisScenarioExt("bpm"),
    m_runExt("bpr"),
    m_unitsSetExt("bpu"),
    m_worksheetExt("bpw"),
    m_xmlFile("BehavePlus6.xml"),
    m_fileList(0)
{
    m_separator = QDir::separator();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Capture Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Capture Folder (with a terminating separator) is constructed.
 *
 *  \return Copy of the Capture Folder file's full path name.
 */

QString FileSystem::capturePath( const QString &fileName )
{
    m_capturePath = m_workspacePath + m_captureFolder + m_separator + fileName;
    return( m_capturePath );
}

//------------------------------------------------------------------------------
/*! \brief Constructs a composer file full path name for the current data
 *  home directory, process id, document id, and page number.
 *
 *  \param docId A document id number unique to this process.
 *  \param pageNo A page number unique to this document.
 *
 *  \return Copy of the composer file's full path name.
 */

QString FileSystem::composerFilePath( int docId, int pageNo )
{
    m_composerFile.sprintf( "%s%s/c%05d%03d%03d.tmp",
        m_workspacePath.latin1(), m_composerFolder.latin1(),
        platformGetPid(), docId, pageNo );
    return( m_composerFile );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Composer Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Composer Folder (with a terminating separator) is constructed.
 *
 *  Called only by isValidWorkspace().
 *
 *  \return Copy of the Composer Folder file's full path name.
 */

QString FileSystem::composerPath( const QString &fileName )
{
    m_composerPath = m_workspacePath + m_composerFolder + m_separator + fileName;
    return( m_composerPath );
}

//------------------------------------------------------------------------------
/*! \brief Gets or sets the current data home directory full path name.
 *
 *  \param newWorkspacePath Reference to the current data home directory
 *  full path name.
 *
 *  if \a newWorkspacePath is NULL or empty, the current data home directory
 *  full path name is returned.
 *
 *  If not NULL or empty, the current data home directory is set to its value.
 *  NOTE that in this case no checking is performed to see if the new path
 *  actually exists, or if it does exist, if it holds anything of use.
 *
 *  \return Copy of the current data home directory full path name.
 */

QString FileSystem::workspacePath( const QString &newWorkspacePath )
{
    if ( ! newWorkspacePath.isNull() && ! newWorkspacePath.isEmpty() )
    {
        m_workspacePath = newWorkspacePath;
        if ( ! m_workspacePath.endsWith( m_separator ) )
        {
            m_workspacePath += m_separator;
        }
    }
    return( m_workspacePath );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Default Workspace Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Default Workspace Folder (with a terminating separator) is constructed.
 *
 *  \return Copy of the Default Workspace Folder file's full path name.
 */

QString FileSystem::defaultWorkspacePath( const QString &fileName )
{
    m_defaultWorkspacePath = m_installPath + m_separator + m_defaultWorkspaceFolder
        + m_separator + fileName;
    return( m_defaultWorkspacePath );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Doc Geo Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Doc Geo Folder (with a terminating separator) is constructed.
 *
 *  \return Copy of the Doc Geo Folder file's full path name.
 */

QString FileSystem::docGeoPath( const QString &fileName )
{
    m_docGeoPath = m_installPath + m_separator + m_docFolder
        + m_separator + m_docGeoFolder + m_separator + fileName;
    return( m_docGeoPath );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's docHtmlIndex file name.
 *
 *  \return Copy of the docHtmlIndex file name.
 */

QString FileSystem::docHtmlIndexFile( void ) const
{
    return( m_docHtmlIndexFile );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's docHtmlIndex file full path name.
 *
 *  \return Copy of the docHtmlIndex file full path name.
 */

QString FileSystem::docHtmlIndexFilePath( void )
{
    m_docHtmlIndexFilePath = docHtmlPath( m_docHtmlIndexFile );
    return( m_docHtmlIndexFilePath );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Doc Html Folder
 *  for the current #m_language.
 *
 *  If \a fileName is empty or NULL, then the full path name of just the
 *  current #m_language Doc Html Folder (with a terminating separator)
 *  is constructed.
 *
 *  \return Copy of the current language's Doc Html Folder file's
 *  full path name.
 */

QString FileSystem::docHtmlPath( const QString &fileName )
{
    m_docHtmlPath = m_installPath + m_separator + m_docFolder + m_separator
        + m_language + m_separator + m_docHtmlFolder + m_separator + fileName;
    return( m_docHtmlPath );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Export Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Export Folder (with a terminating separator) is constructed.
 *
 *  Under the standard BehavePlus file structure, if \a fileName is provided,
 *  then it usually should be a subDir/fileName construct such as
 *  "MyExports/Run1.bpr".
 *
 *  \return Copy of the Export Folder file's full path name.
 */

QString FileSystem::exportPath( const QString &fileName )
{
    m_exportPath = m_workspacePath + m_exportFolder + m_separator + fileName;
    return( m_exportPath );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's features paper file base name.
 *
 *  \return Copy of the features paper file base name.
 */

QString FileSystem::featuresPaperFile( void )
{
    return( m_featuresPaperFile );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's features paper file full path name.
 *
 *  \return Copy of the features paper file full path name.
 */

QString FileSystem::featuresPaperFilePath( void )
{
    m_featuresPaperFilePath = m_installPath + m_separator + m_docFolder
        + m_separator + m_language + m_separator + m_docHtmlFolder
        + m_separator + m_featuresPaperFile;
    return( m_featuresPaperFilePath );
}

//------------------------------------------------------------------------------
/*! \brief Attempts to find the BehavePlus installation directory,
 *  which also contains BehavePlus XML definition file, splash screen image,
 *  and a default data directory.
 *
 *  First the special environment variables are searched:
 *  \arg BEHAVEPLUS
 *  \arg BEHAVEPLUSHOME
 *
 *  Second the first directory of the Windows PATH is checked.
 *
 *  Third, the current working directory is checked.
 *
 *  Since this is invoked before any splash page or XML language definitions
 *  are read, all error messages are in en_US.
 *
 *  \param errMsg String that on return contains any error messages.
 *
 *  \retval TRUE if a valid installation directory was found.  #m_installDir
 *  contains the full installation directory and #m_workspaceDir contains the full
 *  default data directory.
 *
 *  \retval FALSE if no valid installation directory was found.
 */

bool FileSystem::findInstallationDir( QString &errMsg )
{
    log( "Beg Section: Search for Installation Directory:\n" );
    QString end( "End Section: Search for Installation Directory\n\n" );
    // Step 1: First check environment variables that may be set by the user
    //         to override default locations
    //         Applies to both Windows and Linux.
    QString dir("");
    errMsg += "   ";
    const QString EnvVar[] = { "BEHAVEPLUS", "BEHAVEPLUSHOME" };
    for ( int i=0; i<2; i++ )
    {
        dir = getenv( EnvVar[i] );
        if ( findInstallationDir2( EnvVar[i], dir, errMsg ) )
        {
            log( end );
            return( true );
        }
    }

    // Step 2: Check the current working directory
    // Under Linux, the user may invoke the program locally from an alternative
    // directory (such as during program development and debugging).
    // Under Windows, the same may occur, but more likely the user invoked
    // the program from a Desktop or Menu shortcut which has the "Start in"
    // property set (which changes the CWD).
    // Try to get the current working directory
    dir = "";
    char buffer[1024];
    if ( platformGetCwd( buffer, 1024 ) )
    {
        dir = buffer;
    }
    if ( findInstallationDir2( "getcwd()", dir,  errMsg ) )
    {
        log( end );
        return( true );
    }

    // Step 3: Check the location of the executable file
    //         This seems to fail under Windows 95.
    //dir = platformGetExePath( false );
    dir = qApp->applicationDirPath();
    if ( findInstallationDir2( "qApp->applicationDirPath()", dir, errMsg ) )
    {
        log( end );
        return( true );
    }

    // Step 4: Check if the Windows Registry points to a valid installation.
    // This doesn't work, so take it out
    //if ( platformGetOs() == "Windows" )
    //{
    //    dir = platformGetWindowsInstallPath();
    //    if ( findInstallationDir2( "SEM.BehavePlus.2\\shell\\open\\command",
    //        dir, errMsg ) )
    //    {
    //        log( end );
    //        return( true );
    //    }
    //}

    // Couldn't find a valid installation directory.
    // NO TRANSLATOR: the translator is not yet available, so use en_US.
    errMsg = QString( "    A valid installation directory could not be found\n"
        "    during the following process:\n %1" ).arg( errMsg );
    log( end );
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Tests if the specified \a dir is a valid BehavePlus installation
 *  directory with all the required startup and support files.
 *
 *  \retval TRUE if \a dir is a valid BehavePlus installation directory.
 *  \retval FALSE if \a dir is NOT a valid BehavePlus installation directory.
 */

bool FileSystem::findInstallationDir2( const QString &label,
    const QString &dir, QString &errMsg )
{
    QString text = QString( "Checking \"%1\" = \"%2\"\n" )
        .arg( label ).arg( dir );
    log( "Beg Section: " + text );
    QString end = QString( "End Section: " + text );
    QString tmp;

    // Check if the variable is defined ...
    if ( ! dir.isNull() && ! dir.isEmpty() )
    {
        tmp = "Value is DEFINED.\n";
        text += tmp;
        log( tmp );
        // Check for valid installation directory ...
        QString subMsg("");
        if ( isValidInstallation( dir, subMsg ) )
        {
            tmp = "Value is VALID installation directory.\n";
            text += tmp;
            log( tmp );
            // Check for required startup files ...
            if ( findInstallationStartupFiles( dir, errMsg ) )
            {
                tmp = "Required startup files are PRESENT.\n";
                text += tmp;
                log( tmp );
                // Use this as the valid installation directory
                installPath( dir );
                tmp = QString( "ACCEPTED: Install path = \"%1\"\n" )
                    .arg( installPath() );
                text += tmp;
                log( tmp );
                // Return with error message and log the results
                errMsg += text;
                log( end );
                return( true );
            }
            // ... missing required startup files
            else
            {
                tmp = "Required startup files are MISSING.\n";
                text += tmp;
                log( tmp );
            }
        }
        // ... invalid installation directory
        else
        {
            tmp = "Value is an INVALID installation directory.\n";
            text += tmp;
            log( tmp );
            tmp = QString( "Missing required folders or files:\n%1" )
                .arg( subMsg );
            text += tmp;
            log( tmp );
        }
    }
    // ... variable is undefined
    else
    {
        tmp = "Value is UNDEFINED.\n";
        text += tmp;
        log( tmp );
    }
    // This is NOT a valid BehavePlus installation directory.
    tmp = "REJECTED.\n";
    text += tmp;
    log( tmp );

    // Return with error message and log the results
    errMsg += text;
    log( end );
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Checks for the minimum necessary startup files in \a dirName.
 *
 *  BehavePlus requires the following files at startup:
 *      -   a splash page image file in the image directory (Wildfire.png), and
 *      -   the XML file containing all the variable and property definitions
 *          and the text translations (BehavePlus6.xml).
 *
 *  \param dirName  Full path name of the candidate installation directory.
 *  \param errMsg   Reference to the QString to contain any error message.
 *
 *  \retval TRUE if the files are present and readable.
 *  \retval FALSE if either file is absent or not readable, in which case
 *          \a errMsg contains the error message in en_US only.
 */

bool FileSystem::findInstallationStartupFiles( const QString &dirName,
        QString &errMsg )
{
    log( "Beg Section: finding installation startup files ...\n" );
    QString end( "End Section: finding installation startup files.\n" );

    QString pathName[2];
    pathName[0] = dirName + m_separator + m_imageFolder + m_separator
                + m_splashFile;
    pathName[1] = dirName + m_separator + m_xmlFile;
    for ( int i = 0;
          i < 2;
          i++ )
    {
        log( QString( "looking for startup file \"%1\" ...\n" ).arg( pathName[i] ) );
        QFileInfo fi( pathName[i] );
        if ( ! fi.exists() )
        {
            QString tmp = QString( "Required \"%1\" file does not exist." )
                .arg( fi.fileName() );
            errMsg += tmp;
            log( "    does not exist.\n" );
            log( end );
            return( false );
        }
        if ( ! fi.isReadable() )
        {
            QString tmp = QString( "Required \"%1\" file is not readable." )
                .arg( fi.fileName() );
            errMsg += tmp;
            log( "    is not readable.\n" );
            log( end );
            return( false );
        }
        if ( ! fi.isFile() )
        {
            QString tmp = QString( "Required \"%1\" file is not a file." )
                .arg( fi.fileName() );
            errMsg += tmp;
            log( "    is not a file.\n" );
            log( end );
            return( false );
        }
        log( "    found.\n" );
    }
    log( end );
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Finds all the files whose base name matches \a pattern and whose
 *  file extension matches \a extension, and
 *  resides in any subfolder beneath the current workspace \a folder.
 *
 *  \param pattern Base file name pattern.
 *  \param extension File extension pattern.
 *  \param folder Folder to look under.
 *  \param fullPath If true, the full path name is entered into the \a list.
 *  If false, just the subDir\fileName is entered into the \a list.
 *  \param getDescriptions If true, the file description is appended to the
 *  file name entry.
 *  \fileList A QStringList to hold the found file names (and descriptions).
 *
 *  \returns The function returns the number of files found.
 *  The \a fileList contains an entry for each file found.
 *  Each entry contains (1) the file's absolute path name followed by a space
 *  and (2) the file description.
 */

int FileSystem::findFiles( const QString &pattern, const QString &extension,
        const QString &folder, bool fullPath, bool getDescriptions,
        QStringList *fileList )
{
    // Set the fuel folder pathname
    int nFound = 0;
    QDir topFolder( folder );
    QString nameFilter = QString( pattern + "." + extension );
    QString entry, desc;
    // Get a list of all subfolders
    topFolder.setFilter( QDir::Dirs );
    const QFileInfoList *topFolderList = topFolder.entryInfoList();
    if ( ! topFolderList )
    {
        return( false );
    }
    // Loop through the subfolder list
    QFileInfoListIterator tit( *topFolderList );
    QFileInfo *tfi;
    while( ( tfi = tit.current() ) )
    {
        if ( tfi->fileName() == "." || tfi->fileName() == ".." )
        {
            ++tit;
            continue;
        }
        if ( tfi->isDir() && tfi->isReadable() )
        {
            // Set the subfolder path name
            QDir subFolder( tfi->absFilePath() );
            // Get a list of all the .bpf files in the subfolder
            subFolder.setFilter( QDir::Files );
            subFolder.setNameFilter( nameFilter );
            const QFileInfoList *subFolderList = subFolder.entryInfoList();
            QFileInfoListIterator sit( *subFolderList );
            QFileInfo *sfi;
            while( ( sfi = sit.current() ) )
            {
                if ( fullPath )
                {
                    entry = sfi->absFilePath();
                }
                else
                {
                    entry = tfi->fileName() + m_separator + sfi->fileName();
                }
                if ( getDescriptions )
                {
                    fileDescription( sfi->absFilePath(), desc );
                    entry += " ";
                    entry += desc;
                }
                fileList->append( entry );
                nFound++;
                ++sit;
            }
        }
        ++tit;
    }
    return( nFound );
}

//------------------------------------------------------------------------------
/*! \brief Finds all the fuel model files whose base name matches \a pattern,
 *  has the fuel model file extension (.bpf), and
 *  resides in any subfolder beneath the current workspace fuel model folder.
 *
 *  \param pattern Base file name pattern.
 *  \param fullPath If true, the full path name is entered into the \a list.
 *  If false, just the subDir\fileName is entered into the \a list.
 *  \param getDescriptions If true, the file description is appended to the
 *  file name entry.
 *  \fileList A QStringList to hold the found file names (and descriptions).
 *
 *  \returns The function returns the number of files found.
 *  The \a fileList contains an entry for each file found.
 *  Each entry contains (1) the file's absolute path name followed by a space
 *  and (2) the file description.
 */

int FileSystem::findFuelModelFiles( const QString &pattern, bool fullPath,
        bool getDescriptions, QStringList *fileList )
{
    QString folder = QString( m_workspacePath + m_fuelModelFolder );
    return( findFiles( pattern, m_fuelModelExt, folder, fullPath,
        getDescriptions, fileList ) );
}

//------------------------------------------------------------------------------
/*! \brief Finds all the moisture scenario files whose base name matches
 *  \a pattern, has the moisture scenario file extension (.bpf), and resides in
 *  any subfolder beneath the current workspace moisture scenario folder.
 *
 *  \param pattern Base file name pattern.
 *  \param fullPath If true, the full path name is entered into the \a list.
 *  If false, just the subDir\fileName is entered into the \a list.
 *  \param getDescriptions If true, the file description is appended to the
 *  file name entry.
 *  \fileList A QStringList to hold the found file names (and descriptions).
 *
 *  \returns The function returns the number of files found.
 *  The \a fileList contains an entry for each file found.
 *  Each entry contains (1) the file's absolute path name followed by a space
 *  and (2) the file description.
 */

int FileSystem::findMoisScenarioFiles( const QString &pattern, bool fullPath,
        bool getDescriptions, QStringList *fileList )
{
    QString folder = QString( m_workspacePath + m_moisScenarioFolder );
    return( findFiles( pattern, m_moisScenarioExt, folder, fullPath,
        getDescriptions, fileList ) );
}

//------------------------------------------------------------------------------
/*! \brief Finds all the run files whose base name matches \a pattern,
 *  has the run file extension (.bpr), and
 *  resides in any subfolder beneath the current workspace run folder.
 *
 *  \param pattern Base file name pattern.
 *  \param fullPath If true, the full path name is entered into the \a list.
 *  If false, just the subDir\fileName is entered into the \a list.
 *  \param getDescriptions If true, the file description is appended to the
 *  file name entry.
 *  \fileList A QStringList to hold the found file names (and descriptions).
 *
 *  \returns The function returns the number of files found.
 *  The \a fileList contains an entry for each file found.
 *  Each entry contains (1) the file's absolute path name followed by a space
 *  and (2) the file description.
 */

int FileSystem::findRunFiles( const QString &pattern, bool fullPath,
        bool getDescriptions, QStringList *fileList )
{
    QString folder = QString( m_workspacePath + m_runFolder );
    return( findFiles( pattern, m_runExt, folder, fullPath,
        getDescriptions, fileList ) );
}

//------------------------------------------------------------------------------
/*! \brief Finds all the units set files whose base name matches \a pattern,
 *  has the units set file extension (.bpu), and
 *  resides in any subfolder beneath the current workspace units set folder.
 *
 *  \param pattern Base file name pattern.
 *  \param fullPath If true, the full path name is entered into the \a list.
 *  If false, just the subDir\fileName is entered into the \a list.
 *  \param getDescriptions If true, the file description is appended to the
 *  file name entry.
 *  \fileList A QStringList to hold the found file names (and descriptions).
 *
 *  \returns The function returns the number of files found.
 *  The \a fileList contains an entry for each file found.
 *  Each entry contains (1) the file's absolute path name followed by a space
 *  and (2) the file description.
 */

int FileSystem::findUnitsSetFiles( const QString &pattern, bool fullPath,
        bool getDescriptions, QStringList *fileList )
{
    QString folder = QString( m_workspacePath + m_unitsSetFolder );
    return( findFiles( pattern, m_unitsSetExt, folder, fullPath,
        getDescriptions, fileList ) );
}

//------------------------------------------------------------------------------
/*! \brief Finds all the worksheet files whose base name matches \a pattern,
 *  has the worksheet file extension (.bpw), and
 *  resides in any subfolder beneath the current workspace worksheet folder.
 *
 *  \param pattern Base file name pattern.
 *  \param fullPath If true, the full path name is entered into the \a list.
 *  If false, just the subDir\fileName is entered into the \a list.
 *  \param getDescriptions If true, the file description is appended to the
 *  file name entry.
 *  \fileList A QStringList to hold the found file names (and descriptions).
 *
 *  \returns The function returns the number of files found.
 *  The \a fileList contains an entry for each file found.
 *  Each entry contains (1) the file's absolute path name followed by a space
 *  and (2) the file description.
 */

int FileSystem::findWorksheetFiles( const QString &pattern, bool fullPath,
        bool getDescriptions, QStringList *fileList )
{
    QString folder = QString( m_workspacePath + m_worksheetFolder );
    return( findFiles( pattern, m_worksheetExt, folder, fullPath,
        getDescriptions, fileList ) );
}

//------------------------------------------------------------------------------
/*! \brief Gets the standard Fuel Model file extension.
 *
 *  \return Copy of the standard Fuel Model file extension.
 */

QString FileSystem::fuelModelExt( void ) const
{
    return( m_fuelModelExt );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Fuel Model Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Fuel Model Folder (with a terminating separator) is constructed.
 *
 *  Under the standard BehavePlus file structure, if \a fileName is provided,
 *  then it usually should be a subDir/fileName construct such as
 *  "MyFuelModels/Model14.bpf".
 *
 *  \return Copy of the Fuel Model Folder file's full path name.
 */

QString FileSystem::fuelModelPath( const QString &fileName )
{
    m_fuelModelPath = m_workspacePath + m_fuelModelFolder + m_separator + fileName;
    return( m_fuelModelPath );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the specified geographic file exists and is readable.
 *
 *  \return TRUE if fileName exists in the geographic Folder and is readable.
 */

bool FileSystem::geoFileExists( const QString &fileName, QString &errMsg )
{
    QString pathName = docGeoPath( fileName );
    QFileInfo fi( pathName );
    if ( ! fi.exists() || ! fi.isReadable() || ! fi.isFile() )
    {
        QString msg("");
        translate( msg, "FileSystem:missingFile", pathName );
        errMsg += "<BR> " + msg;
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's help file base name.
 *
 *  \return Copy of the help file base name.
 */

QString FileSystem::helpFile( void )
{
    return( m_helpFile );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's help file full path name.
 *
 *  \return Copy of the help file full path name.
 */

QString FileSystem::helpFilePath( void )
{
    m_helpFilePath = m_installPath + m_separator + m_docFolder
        + m_separator + m_language + m_separator + m_docHtmlFolder
        + m_separator + m_helpFile;
    return( m_helpFilePath );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the specified HTML file exists and is readable.
 *
 *  \return TRUE if fileName exists in the HTML Folder and is readable.
 */

bool FileSystem::htmlFileExists( const QString &fileName, QString &errMsg )
{
    QString pathName = docHtmlPath( fileName );
    QFileInfo fi( pathName );
    if ( ! fi.exists() || ! fi.isReadable() || ! fi.isFile() )
    {
        QString msg("");
        translate( msg, "FileSystem:missingFile", pathName );
        errMsg += "<BR> " + msg;
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the specified image file exists and is readable.
 *
 *  \return TRUE if fileName exists in the Image Folder and is readable.
 */

bool FileSystem::imageFileExists( const QString &fileName, QString &errMsg )
{
    QString pathName = imagePath( fileName );
    QFileInfo fi( pathName );
    if ( ! fi.exists() || ! fi.isReadable() || ! fi.isFile() )
    {
        errMsg = QString( "\"%1\" file is missing or not readable." )
            .arg( fileName );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Image Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Image Folder (with a terminating separator) is constructed.
 *
 *  \return Copy of the Image Folder file's full path name.
 */

QString FileSystem::imagePath( const QString &fileName )
{
    m_imagePath = m_installPath + m_separator + m_imageFolder
        + m_separator + fileName;
    return( m_imagePath );
}

//------------------------------------------------------------------------------
/*! \brief Gets or sets the installation home directory full path name.
 *
 *  \param newInstallPath Reference to the new installation home directory
 *  full path name.
 *
 *  If \a newInstallPath is NULL or empty, it merely returns the installation
 *  home directory full path name.
 *
 *  If \a newInstallPath is not NULL or empty, it sets the installation home
 *  directory to its value. NOTE that in this case no checking is performed
 *  to see if the new path actually exists, or if it does exist,
 *  if it holds anything of use.
 *
 *  \return Copy of the installation home directory full path name.
 */

QString FileSystem::installPath( const QString &newInstallPath )
{
    if ( ! newInstallPath.isNull() && ! newInstallPath.isEmpty() )
    {
        m_installPath = newInstallPath;
    }
    return( m_installPath );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the named directory exists and is readable.
 *
 *  Called by FileSystem::isValidInstallation().
 *
 *  \param dirName Directory name.
 *  \param errMsg Error message to be built upon.
 *
 *  \retval TRUE if the directory exists and is readable and writable.
 *  \retval FALSE if the directory doesn't exist or isn't readable or writable.
 */

bool FileSystem::isValidDirectory( const QString &dirName, QString &errMsg )
{
    QDir dir( dirName );
    QString msg("");
    if ( ! dir.exists() )
    {
        errMsg +=
            QString( "            missing directory " + dirName + "\n" );
        return( false );
    }
    if ( ! dir.isReadable() )
    {
        errMsg +=
            QString( "            unreable directory " + dirName + "\n" );
        return( false );
    }
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Performs a cursory examination of the passed directory name to
 *      determine if it has a valid BehavePlus installation structure.
 *
 *  Called by findInstallationDir(). and by
 *  ApplicationWindow::findInstallation() BEFORE the XML file has been located
 *      and read, so we don't yet have the complete file list.
 *
 *  \param installPath Full installation directory name.
 *  \param errMsg returned error messages.
 *
 *  A valid BehavePlus \b workspace \b directory has the following components:
 *  \arg \c CaptureFolder
 *  \arg \c ComposerFolder
 *  \arg \c FuelModelFolder
 *  \arg \c MoistureScenarioFolder
 *  \arg \c RunFolder
 *  \arg \c UnitsSetFolder
 *  \arg \c WorksheetFolder
 *
 *  A valid BehavePlus \b installation \b directory contains
 *  \arg \c DefaultWorkspace (containing a complete \b data \b directory)
 *  \arg \c DocsFolder\Geo
 *  \arg \c DocsFolder\m_language\en_US
 *  \arg \c ImageFolder
 *
 *  \retval TRUE if the installation is valid.
 *  \retval FALSE if the installation is invalid
 */

bool FileSystem::isValidInstallation( const QString &installPath,
        QString &errMsg )
{
    // Store the current installation directory
    QString currentInstallPath = m_installPath;
    m_installPath = installPath;

    // Test all the directories and accumulate the error message
    bool ok = true;
    ok &= isValidDirectory( m_installPath, errMsg );
    ok &= isValidDirectory( docGeoPath(), errMsg );
    ok &= isValidDirectory( docHtmlPath(), errMsg );
    ok &= isValidDirectory( imagePath(), errMsg ) ;
    ok &= isValidDirectory( defaultWorkspacePath(), errMsg ); ;

    // Restore the installation path
    m_installPath = currentInstallPath;
    return( ok );
}

//------------------------------------------------------------------------------
/*! \brief Gets or sets the current documentation language folder.
 *
 *  \param newLanguage Reference to the new language directory folder name.
 *
 *  If \a newLanguage is NULL or empty, it merely returns the current
 *  language folder name (NOT the full path name).
 *
 *  If \a newLanguage is not NULL or empty, it sets the language folder
 *  to its value. NOTE that in this case no checking is performed
 *  to see if the new folder actually exists, or if it does exist,
 *  if it holds anything of use.
 *
 *  \return Copy of the current language folder name.
 */

QString FileSystem::language( const QString &newLanguage )
{
    if ( newLanguage.isNull() || newLanguage.isEmpty() )
    {
        return( m_language );
    }
    return( m_language = newLanguage );
}

//------------------------------------------------------------------------------
/*! \brief Makes a new workspace by creating the required directories
 *  and copying the required files from the current (valid) workspace.
 *
 *  \param newWorkspacePath New (destination) workspace full pathname.
 *  \param srcWorkspacePath Source workspace full pathname.
 *      \param clone If TRUE, all files are copied from the source to the
 *                                      destination.  If FALSE, only mandated files are copied
 *                                      and the new workspace looks like a new installed one.
 *  \param errMsg   String that on returns contains any error message.
 *
 *  \retval TRUE if \a workspacePath is created successfully.
 *  \retval FALSE if \a workspacePath is not created successfully,
 *  and the error message is in \a errMsg.
 */

bool FileSystem::makeWorkspace( const QString &newWorkspacePath,
        const QString &srcWorkspacePath, bool clone, QString &errMsg )
{
    // Set up the progress dialog.
    QString caption(""), button("");
    translate( caption, "FileSystem:MakeWorkspace:Progress:Caption" );
    translate( button, "FileSystem:MakeWorkspace:Progress:Button" );
    QProgressDialog *progress = new QProgressDialog( caption, button,
        2 * m_fileList->count() );
    Q_CHECK_PTR( progress );
    progress->setMinimumDuration( 0 );
    int step = 0;
    progress->setProgress( step );
        progress->show();
    qApp->processEvents();

    // Store soure and destination pathnames with terminating separator
    QString srcWorkspace = srcWorkspacePath;
    if ( ! srcWorkspace.endsWith( m_separator ) )
    {
        srcWorkspace += m_separator;
    }
    QString dstWorkspace = newWorkspacePath;
    if ( ! dstWorkspace.endsWith( m_separator ) )
    {
        dstWorkspace += m_separator;
    }

    // If this is a clone ...
    if ( clone )
    {
        progress->setProgress( m_fileList->count() / 2 );
        qApp->processEvents();
        bool status = subdirCopy( srcWorkspace, dstWorkspace, errMsg );
        // Update "progress" dialog.
        progress->setProgress( 2 * m_fileList->count() );
        qApp->processEvents();
        delete progress;    progress = 0;
        return( status );
    }

    // Make all the destination subdirectories
    QDir dir( dstWorkspace );
    QString srcPath, dstPath;
    int release = appWindow()->m_release;
    EqFile *filePtr;
    for ( filePtr = m_fileList->first();
          filePtr != 0;
          filePtr = m_fileList->next() )
    {
        // Create subdirectory
        if ( filePtr->m_type == EqFileType_Dir
          && filePtr->isCurrent( release ) )
        {
            dstPath = dstWorkspace + filePtr->m_name;
            if ( ! dir.mkdir( dstPath, true ) )
            {
                delete progress;    progress = 0;
                translate( errMsg, "FileSystem:MakeWorkspace:Error", dstPath );
                return( false );
            }
        }
        // Update progress dialog.
        progress->setProgress( ++step );
        qApp->processEvents();
        if ( progress->wasCancelled() )
        {
            delete progress;    progress = 0;
            return( false );
        }
    }

    // Copy all the required files
    for ( filePtr = m_fileList->first();
          filePtr != 0;
          filePtr = m_fileList->next() )
    {
        if ( ( filePtr->m_type == EqFileType_BpData
            || filePtr->m_type == EqFileType_BpDesc )
          && filePtr->isCurrent( release ) )
        {
            srcPath = srcWorkspace + filePtr->m_name;
            dstPath = dstWorkspace + filePtr->m_name;
            if ( ! textFileCopy( srcPath, dstPath, errMsg ) )
            {
                delete progress;    progress = 0;
                return( false );
            }
        }
        // Update progress dialog.
        progress->setProgress( ++step );
        qApp->processEvents();
        if ( progress->wasCancelled() )
        {
            delete progress;    progress = 0;
            return( false );
        }
    }
        // Copy all the shared files

    // Restore the data directory and return.
    delete progress;    progress = 0;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Gets the standard Moisture Scenario file extension.
 *
 *  \return Copy of the standard Moisture Scenario file extension.
 */

QString FileSystem::moisScenarioExt( void ) const
{
    return( m_moisScenarioExt );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Moisture Scenario
 *  Folder.
 *
 *  \param fileName If empty or NULL, then the full path name of just the
 *  Moisture Scenario Folder (with a terminating separator) is constructed.
 *
 *  Under the standard BehavePlus file structure, if \a fileName is provided,
 *  then it usually should be a subDir/fileName construct such as
 *  "MyMoistureScenarios/Scenario1.bpm".
 *
 *  \return Copy of the Moisture Scenario Folder file's full path name.
 */

QString FileSystem::moisScenarioPath( const QString &fileName )
{
    m_moisScenarioPath = m_workspacePath + m_moisScenarioFolder + m_separator
        + fileName;
    return( m_moisScenarioPath );
}

//------------------------------------------------------------------------------
/*! \brief Pretty-prints all the HTML files for this release.
 *
 *  Usually called only by AppWindow::checkCommandLineSwitches( void )
 *      when "-html" is specified on the command line.
 */

void FileSystem::printHtmlFiles( QWidget* parent )
{
    TextBrowser* browserPtr = new TextBrowser( parent, "htmlPrinter" );

    int release = appWindow()->m_release;
    EqFile *filePtr;
    for ( filePtr = m_fileList->first();
          filePtr != 0;
          filePtr = m_fileList->next() )
    {
        if ( filePtr->m_type == EqFileType_DocHtml
          && filePtr->isCurrent( release ) )
        {
            fprintf( stderr, "About to print %s ...\n", filePtr->m_name.latin1() );
            browserPtr->mimeSourceFactory()->setFilePath( QStringList( docHtmlPath() ) );
            browserPtr->setSource( filePtr->m_name );
            if ( ! printRichText( browserPtr ) )
            {
                return;
            }
        }
    }
    delete browserPtr;  browserPtr = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's property file full path name.
 *
 *  \return Copy of the property file full path name.
 */

QString FileSystem::propertyFilePath( void )
{
    m_propertyFilePath = m_workspacePath + m_propertyFile;
    return( m_propertyFilePath );
}

//------------------------------------------------------------------------------
/*! \brief Gets the standard Run file extension.
 *
 *  \return Copy of the standard Run file extension.
 */

QString FileSystem::runExt( void ) const
{
    return( m_runExt );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Run Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Run Folder (with a terminating separator) is constructed.
 *
 *  Under the standard BehavePlus file structure, if \a fileName is provided,
 *  then it usually should be a subDir/fileName construct such as
 *  "MyRuns/Run1.bpr".
 *
 *  \return Copy of the Run Folder file's full path name.
 */

QString FileSystem::runPath( const QString &fileName )
{
    m_runPath = m_workspacePath + m_runFolder + m_separator + fileName;
    return( m_runPath );
}

//------------------------------------------------------------------------------
/*! \brief Constructs a temporary file full path name for the current data
 *  home directory, process id, and document id.
 *
 *  \param docId A document id number unique to this process.
 *
 *  \return Copy of the composer file's full path name.
 */

QString FileSystem::tempFilePath( int docId )
{
    QString tempFile("");
    tempFile.sprintf( "%s%s/t%05d%05d.tmp",
        m_workspacePath.latin1(), m_composerFolder.latin1(), platformGetPid(), docId );
    return( tempFile );
}

//------------------------------------------------------------------------------
/*! \brief Tests the existence of all distribution Geographic Folder files.
 *
 *  \param errMsg Reference to a string where error messages are appended.
 *
 *  \return TRUE if all files are present, FALSE if any are missing.
 *  On return the errMsg has all the missing files listed.
 */

bool FileSystem::testGeoFiles( QString &errMsg )
{
    bool ok = true;
    int release = appWindow()->m_release;
    EqFile *filePtr;
    for ( filePtr = m_fileList->first();
          filePtr != 0;
          filePtr = m_fileList->next() )
    {
        if ( filePtr->m_type == EqFileType_GeoData
          && filePtr->isCurrent( release ) )
        {
            ok &= geoFileExists( filePtr->m_name, errMsg );
        }
    }
    return( ok );
}

//------------------------------------------------------------------------------
/*! \brief Tests the existence of all distribution Html Folder files.
 *  On return the errMsg has all the missing files listed.
 *
 *  \return TRUE if all files are present, FALSE if any are missing.
 */

bool FileSystem::testHtmlFiles( QString &errMsg )
{
    bool ok = true;
    int release = appWindow()->m_release;
    EqFile *filePtr;
    for ( filePtr = m_fileList->first();
          filePtr != 0;
          filePtr = m_fileList->next() )
    {
        if ( ( filePtr->m_type == EqFileType_DocHtml
                || filePtr->m_type == EqFileType_DocHlp
                || filePtr->m_type == EqFileType_DocPdf
            || filePtr->m_type == EqFileType_DocImage )
          && filePtr->isCurrent( release ) )
        {
            ok &= htmlFileExists( filePtr->m_name, errMsg );
        }
    }
    return( ok );
}

//------------------------------------------------------------------------------
/*! \brief Tests the existence of all distribution Image Folder files.
 *
 *  \return TRUE if all files are present, FALSE if any are missing.
 */

bool FileSystem::testImageFiles( QString &errMsg )
{
    bool ok = true;
    int release = appWindow()->m_release;
        EqFile *filePtr;
    for ( filePtr = m_fileList->first();
          filePtr != 0;
          filePtr = m_fileList->next() )
    {
        if ( filePtr->m_type == EqFileType_DialogImage
          && filePtr->isCurrent( release ) )
        {
            ok &= imageFileExists( filePtr->m_name, errMsg );
                }
    }
    return( ok );
}

//------------------------------------------------------------------------------
/*! \brief Tests the existence of all distribution installation files.
 *
 *  Since this uses the file list, it must be called after the XML file has
 *  been read in.
 *
 *  \return TRUE if all files are present, FALSE if any are missing.
 */

bool FileSystem::testWorkspaceFiles( const QString &workspacePath,
                QString &errMsg )
{
    bool ok = true;
    int release = appWindow()->m_release;
    QString pathName, type;
    QFileInfo fi;
    int warnings = 0;
    // Loop through the entire file list
    EqFile *filePtr;
    for ( filePtr = m_fileList->first();
          filePtr != 0;
          filePtr = m_fileList->next() )
    {
        type = " file ";
        if ( ! filePtr->isCurrent( release ) )
        {
            continue;
        }
        else if ( filePtr->m_type == EqFileType_Dir )
        {
            type = " directory ";
            pathName = workspacePath + filePtr->m_name;
        }
        else if ( filePtr->m_type == EqFileType_BpData )
        {
            pathName = workspacePath + filePtr->m_name;
        }
        else if ( filePtr->m_type == EqFileType_BpDesc )
        {
            pathName = workspacePath + filePtr->m_name;
        }
        else
        {
            continue;
        }
        fi.setFile( pathName );

        // HACK to maintain backward compatability with evolving directory names:
        // Version 1.0.0 WorksheetFolder/Standard -->
        // Version 2.0.a WorksheetFolder/Example -->
        // Version 2.0.0 WorksheetFolder/ExampleWorksheet -->
        // Version 1.0.0 WorksheetFolder/Standard/Blank.bpw -->
        // Version 2.0.a WorksheetFolder/Example/0Startup.bpw -->
        // Version 2.0.0 WorksheetFolder/ExampleWorksheet/0Startup.bpw -->
        // Version 5.0.0 WorksheetFolder/ExampleWorksheet/0Default.bpw .

        // Check if it must exist...
        if ( filePtr->m_permission.contains( "E" ) )
        {
            if ( ! fi.exists() )
            {
                // Ignore missing ExampleWorksheet folder and files
                // under the assumption they exist under old names
                if ( pathName.contains( "/ExampleWorksheets" ) )
                {
                    warnings++;
                }
                else
                {
                    errMsg += QString( "            missing required" + type
                           + pathName + "\n" );
                    ok = false;
                }
                continue;
            }
        }
        if ( filePtr->m_permission.contains( "R" ) )
        {
            if ( ! fi.isReadable() )
            {
                errMsg += QString( "            unreadable" + type
                       + pathName + "\n" );
                ok = false;
            }
            continue;
        }
        if ( filePtr->m_permission.contains( "W" ) )
        {
            if ( ! fi.isWritable() )
            {
                errMsg += QString( "            unwritable" + type
                       + pathName + "\n" );
                ok = false;
            }
                continue;
        }
    }
    //fprintf( stderr, "installPath = [%s]\n", installPath().latin1() );
    return( ok );
}

//------------------------------------------------------------------------------
/*! \brief Sets the EqFile list of all files and directories.
 *
 *  Called only by AppWindow::AppWindow() after reading the XML file.
 */

void FileSystem::setFileList( QPtrList<EqFile> *fileList )
{
    m_fileList = fileList;
}

//------------------------------------------------------------------------------
/*! \brief Gets the standard file type from the file extension.
 *
 *  \return One of "Run", "Worksheet", "Fuel Model", "Units Set",
 *          "Fuel Model", "Moisture Scenario", or "".
 */

QString FileSystem::type( const QString &fileName ) const
{
    if ( fileName.endsWith( m_runExt ) )
    {
        return( "Run" );
    }
    else if ( fileName.endsWith( m_worksheetExt ) )
    {
        return( "Worksheet" );
    }
    else if ( fileName.endsWith( m_worksheetExt ) )
    {
        return( "Worksheet" );
    }
    else if ( fileName.endsWith( m_fuelModelExt ) )
    {
        return( "Fuel Model" );
    }
    else if ( fileName.endsWith( m_moisScenarioExt ) )
    {
        return( "Moisture Scenario" );
    }
    else if ( fileName.endsWith( m_unitsSetExt ) )
    {
        return( "Units Set" );
    }
    return( "" );
}

//------------------------------------------------------------------------------
/*! \brief Gets the standard Units Set file extension.
 *
 *  \return Copy of the Units Set file extension.
 */

QString FileSystem::unitsSetExt( void ) const
{
    return( m_unitsSetExt );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Units Sets Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Units Sets Folder (with a terminating separator) is constructed.
 *
 *  Under the standard BehavePlus file structure, if \a fileName is provided,
 *  then it usually should be a subDir/fileName construct such as
 *  "MyUnitsSets/Set1.bpu".
 *
 *  \return Copy of the Units Sets Folder file's full path name.
 */

QString FileSystem::unitsSetPath( const QString &fileName )
{
    m_unitsSetPath = m_workspacePath + m_unitsSetFolder + m_separator + fileName;
    return( m_unitsSetPath );
}

//------------------------------------------------------------------------------
/*! \brief Gets the unnamed document base file name.
 *
 *  \return Copy of the unnamed document base file name.
 */

QString FileSystem::unNamedFile( void ) const
{
    return( m_unNamedFile );
}

//------------------------------------------------------------------------------
/*! \brief Forces use of the default data folder as the current data folder.
 *
 *  Typically called at startup once the installation directory is found
 *  and validated so that we have a default data folder.
 *
 *  The data CURRENT directory may subsequently be reset via a call to
 *  workspacePath( const QString &newWorkspacePath ).
 *
 *  \return Copy of the default/current data directory full path name.
 */

QString FileSystem::useDefaultWorkspace( void )
{
    return( workspacePath( defaultWorkspacePath() ) );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's user guide file base name.
 *
 *  \return Copy of the user guide file base name.
 */

QString FileSystem::userGuideFile( void )
{
    return( m_userGuideFile );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's user guide file full path name.
 *
 *  \return Copy of the user guide file full path name.
 */

QString FileSystem::userGuideFilePath( void )
{
    m_userGuideFilePath = m_installPath + m_separator + m_docFolder
        + m_separator + m_language + m_separator + m_docHtmlFolder
        + m_separator + m_userGuideFile;
    return( m_userGuideFilePath );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's variable reference file base name.
 *
 *  \return Copy of the variable reference file base name.
 */

QString FileSystem::variableRefFile( void )
{
    return( m_variableRefFile );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's variable reference file full path name.
 *
 *  \return Copy of the variable reference file full path name.
 */

QString FileSystem::variableRefFilePath( void )
{
    m_variableRefFilePath = m_installPath + m_separator + m_docFolder
        + m_separator + m_language + m_separator + m_docHtmlFolder
        + m_separator + m_variableRefFile;
    return( m_variableRefFilePath );
}

//------------------------------------------------------------------------------
/*! \brief Gets the standard Worksheet file extension.
 *
 *  \return Copy of the standard Worksheet file extension.
 */

QString FileSystem::worksheetExt( void ) const
{
    return( m_worksheetExt );
}

//------------------------------------------------------------------------------
/*! \brief Builds a full path name to \a fileName in the Worksheet Folder.
 *
 *  \param fileName If empty or NULL, the full path name of just the
 *  Worksheet Folder (with a terminating separator) is constructed.
 *
 *  Under the standard BehavePlus file structure, if \a fileName is provided,
 *  then it usually should be a subDir/fileName construct such as
 *  "MyWorksheets/Worksheet1.bpw".
 *
 *  \return Copy of the Worksheet Folder file's full path name.
 */

QString FileSystem::worksheetPath( const QString &fileName )
{
    m_worksheetPath = m_workspacePath + m_worksheetFolder + m_separator + fileName;
    return( m_worksheetPath );
}


//------------------------------------------------------------------------------
/*! \brief Gets the application's Xml file base name.
 *
 *  \return Copy of the XML file base name.
 */

QString FileSystem::xmlFile( void )
{
    return( m_xmlFile );
}

//------------------------------------------------------------------------------
/*! \brief Gets the application's Xml file full path name.
 *
 *  \return Copy of the XML file full path name.
 */

QString FileSystem::xmlFilePath( void )
{
    m_xmlFilePath = m_installPath + m_separator + m_xmlFile;
    return( m_xmlFilePath );
}

//------------------------------------------------------------------------------
/*! \brief Guarantees that \a fileName ends with \a ext.
 *
 *  \retval TRUE if \a ext was appended to \a fileName.
 *  \retval FALSE if \a ext was NOT appnded to \a fileName because it already
 *  ends with it.
 */

bool addExtension( QString &fileName, const QString &ext )
{
    QFileInfo fi( fileName );
    if ( fi.extension( false ) == ext )
    {
        return( false );
    }
    fileName = fileName + "." + ext;
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Copies one sudirectory tree into another.
 *
 *  \retval TRUE on success, FALSE on failure.
 */

bool subdirCopy( const QString &srcPath, const QString &dstPath,
        QString &errMsg )
{
        // Make the destination directory here
        QDir dir;
        dir.setPath( dstPath );
        //fprintf( stderr, "mkdir %s\n", dir.absPath().latin1() );
        dir.mkdir( dir.absPath() );

        dir.setPath( srcPath );
        dir.setFilter( QDir::Dirs | QDir::Files | QDir::NoSymLinks );
        const QFileInfoList* fileInfoList = dir.entryInfoList();
        if ( fileInfoList )
        {
                QFileInfo* fi;
                QFileInfoListIterator it( *fileInfoList );
                while( ( fi = it.current() ) != 0 )
                {
                        ++it;
                        if ( fi->fileName() == "."
                          || fi->fileName() == ".." )
                        {
                                continue;
                        }
                        if ( fi->isDir() )
                        {
                                if ( fi->isReadable() )
                                {
                                        QString folder = dstPath + QDir::separator() + fi->baseName();
                                        if ( ! subdirCopy( fi->absFilePath(), folder, errMsg ) )
                                        {
                                                return( false );
                                        }
                                }
                        }
                        else
                        {
                                QString src = fi->absFilePath();
                                QString dst = dstPath + QDir::separator() + fi->fileName();
                                //fprintf( stderr, "cp %s %s\n", src.latin1(), dst.latin1() );
                                if ( ! textFileCopy( src, dst, errMsg ) )
                                {
                                        return( false );
                                }
                        }
                }
        }
        return( true );
}

//------------------------------------------------------------------------------
/*! \brief Copies a text file.
 *
 *  \param srcPath Full path name of the source text file.
 *  \param dstPath Full path name of the destination text file.
 *  \param errMsg  On return contains any error message
 *
 *  \return TRUE if successful.
 *  \retval FALSE if unsuccessful, and \a errMsg describes the error.
 */

bool textFileCopy( const QString &srcPath, const QString &dstPath,
        QString &errMsg )
{
    errMsg = "";
    // Source must be an existing, readable file
    QFileInfo srcInfo( srcPath );
    if ( ! srcInfo.exists() )
    {
        translate( errMsg, "TextFileCopy:Source:NotFound", srcPath );
        return( false );
    }
    if ( ! srcInfo.isReadable() )
    {
        translate( errMsg, "TextFileCopy:Source:NotReadable", srcPath );
        return( false );
    }
    if ( ! srcInfo.isFile() )
    {
        translate( errMsg, "TextFileCopy:Source:NotAFile", srcPath );
        return( false );
    }
    // If destination exists, it must be a writable file
    QFileInfo dstInfo( dstPath );
    if ( dstInfo.exists() )
    {
        if ( ! dstInfo.isWritable() )
        {
            translate( errMsg, "TextFileCopy:Dest:NotWritable", dstPath );
            return( false );
        }
        if ( ! dstInfo.isFile() )
        {
            translate( errMsg, "TextFileCopy:Dest:NotAFile", dstPath );
            return( false );
        }
    }
    // Open the source file
    QFile srcFile( srcPath );
    if ( ! srcFile.open( IO_ReadOnly ) )
    {
        translate( errMsg, "TextFileCopy:Source:NoOpen", srcPath );
        return( false );
    }
    // Open the destination file
    QFile dstFile( dstPath );
    if ( ! dstFile.open( IO_WriteOnly ) )
    {
        translate( errMsg, "TextFileCopy:Dest:NoOpen", dstPath );
        return( false );
    }
    // Copy source to destination
    QTextStream src( &srcFile );
    QTextStream dst( &dstFile );
    QString line;
    while ( ! src.eof() )
    {
        line = src.readLine();
        dst << line << endl;
    }
    // Close files and return
    srcFile.close();
    dstFile.close();
    return( true );
}

//------------------------------------------------------------------------------
/*! \brief Opens a BehavePlus file, reads its description, and returns it
 *  in \a desc.
 *
 *  \param fileName Full BehavePlus document file path name.
 *  \param desc Reference to QString to hold the returned text.
 *
 *  \retval 0 if the file description was found.
 *  \retval -1 if the file could not be opened.
 *  \retval -2 if the file is not a BehavePlus file.
 *  \retval -3 if the description was not found.
 */

int fileDescription( const char *fileName, QString &desc )
{
    FILE *fptr;
    char  buffer[BIG_BUFFER], value[SMALL_BUFFER];

    // Try to open the file.
    if ( ! ( fptr = fopen( fileName, "r" ) ) )
    {
        return( -1 );
    }

    // The first line is the BehavePlus header
    int line = 0;
    char *ptr;
    memset( buffer, '\0', sizeof(buffer) );
    bool behavePlusDocument = false;
    while ( fgets( buffer, sizeof(buffer), fptr ) )
    {
        line++;
        if ( strstr( buffer, "<BehavePlus type=" ) )
        {
            behavePlusDocument = true;
        }
        // Check for the app description
        else if ( ( ptr = strstr( buffer, "name=\"appDescription\"" ) ) )
        {
            if ( ( ptr = strstr( buffer, "value=\"" ) ) )
            {
                memset( value, '\0', sizeof(value) );
                sscanf( ptr+7, "%[^\"]", value );
                if ( strcmp( value, "(null)" ) == 0 )
                {
                    desc = "";
                }
                else
                {
                    desc = value;
                    replaceXmlEntities( desc );
                }
            }
            fclose( fptr );
            return( 0 );
        }
        //  Skip empty lines or the first character is not alphabetic.
    }
    fclose( fptr );
    if ( ! behavePlusDocument )
    {
        return( -2 );
    }
    return( -3 );
}

//------------------------------------------------------------------------------
/*! \brief Opens a BehavePlus folder description file, reads its description,
 *  and returns it in \a desc.
 *
 *  \param fileName Full BehavePlus folder path name.
 *  \param desc Reference to QString to hold the returned text.
 *
 *  \retval 0 if the folder description was found.
 *  \retval -1 if the folder's description file could not be opened.
 */

int folderDescription( QString &fileName, QString &desc )
{
    desc = "";

    // Attempt to open the file.
    QFile file( fileName );
    if ( ! file.open( IO_ReadOnly ) )
    {
        return( -1 );
    }

    // Read just the first line into the string, close the file, and return
    QTextStream textStream( &file );
    desc = textStream.readLine();
    file.close();
    return( 0 );
}

//------------------------------------------------------------------------------
/*! \brief Determines if the file is a BehavePlus XML document file.
 *
 *  \return TRUE if it is a BehavePlus document file, otherwise FALSE.
 */

bool isBehavePlusXmlDocument( const QString &fileName )
{
    // Try to open the file.
    FILE *fptr;
    if ( ! ( fptr = fopen( (const char *) fileName, "r" ) ) )
    {
        return( false );
    }
    char buffer[1024];
    while ( fgets( buffer, sizeof(buffer), fptr ) )
    {
        if ( strstr( buffer, "<BehavePlus type=" ) )
        {
            fclose( fptr );
            return( true );
        }
    }
    fclose( fptr );
    return( false );
}

//------------------------------------------------------------------------------
/*! \brief Creates an HTML table of the file names and descriptions contained
 *  in \a list, which is normally generated by a call to one of the
 *  FileSystem::find<*>Files() methods.
 *
 *  \param list
 *  \param table Reference to a QString which on return contains the HTML
 *  file table.
 *
 *  \returns  This function returns nothing.  The HTML table is returned
 *  in \a table.
 */

void makeFileTable( QStringList &list, const QString &title, QString &table )
{
    // Get the titles
    QString fileNameHdr(""), fileDescHdr("");
    translate( fileNameHdr, "FileSystem:fileNameHdr" );
    translate( fileDescHdr, "FileSystem:fileDescHdr" );
    // Write table header
    table = QString( "<H2>%1 (%2)</H2><TABLE>" )
        .arg( title ).arg( list.count() );
    table += QString( "<TR><TH>%1</TH><TH>%2</TH>" )
        .arg( fileNameHdr ).arg( fileDescHdr );
    // Write each file's name and description
    QStringList::Iterator it;
    QString name, desc;
    int pos;
    for ( it = list.begin();
          it != list.end();
          ++it )
    {
        if ( ( pos = (*it).find( " " ) ) >= 0 )
        {
            name = (*it).left( pos );
            desc = (*it).mid( pos+1 );
        }
        else
        {
            name = (*it);
            desc = "";
        }
        table += QString( "<TR><TD>%1</TD><TD>%2</TD>" )
            .arg( name ).arg( desc );
    }
    table += "</TABLE>";
    return;
}

//------------------------------------------------------------------------------
/*! \brief Replaces all XML entities with their ASCII characters.
 *
 *  \param text Reference to QString to hold the returned text.
 */

void replaceXmlEntities( QString &text )
{
    int pos = 0;
    while ( ( pos = text.find( "&amp;", 0, false ) ) >= 0 )
    {
        text = text.replace( pos, 5, "&" );
    }
    while ( ( pos = text.find( "&quot;", 0, false ) ) >= 0 )
    {
        text = text.replace( pos, 6, "\"" );
    }
    while ( ( pos = text.find( "&apos;", 0, false ) ) >= 0 )
    {
        text = text.replace( pos, 6, "'" );
    }
    while ( ( pos = text.find( "&lt;", 0, false ) ) >= 0 )
    {
        text = text.replace( pos, 4, "<" );
    }
    while ( ( pos = text.find( "&gt;", 0, false ) ) >= 0 )
    {
        text = text.replace( pos, 4, ">" );
    }
    return;
}

//------------------------------------------------------------------------------
//  End of filesys.cpp
//------------------------------------------------------------------------------

