/********************************************************************************
 * Copyright 2009 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
 * Faculty of Engineering, University of Southern Denmark
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ********************************************************************************/

#define QT_NO_EMIT
#include "RobWorkStudio.hpp"

#include "AboutBox.hpp"
#include "HelpAssistant.hpp"
#include "RobWorkStudioPlugin.hpp"

#include <rw/common/TimerUtil.hpp>
#include <rw/core/Exception.hpp>
#include <rw/core/StringUtil.hpp>
#include <rw/core/os.hpp>
#include <rw/geometry/BSphere.hpp>
#include <rw/kinematics/StateStructure.hpp>
#include <rw/loaders/WorkCellLoader.hpp>
#include <rw/loaders/dom/DOMPropertyMapLoader.hpp>
#include <rw/loaders/dom/DOMPropertyMapSaver.hpp>
#include <rw/loaders/dom/DOMWorkCellSaver.hpp>
#include <rw/loaders/rwxml/XMLRWLoader.hpp>
#include <rw/models/Object.hpp>
#include <rw/models/WorkCell.hpp>
#include <rw/proximity/CollisionDetector.hpp>
#include <rw/proximity/CollisionSetup.hpp>
#include <rwlibs/proximitystrategies/ProximityStrategyFactory.hpp>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QIcon>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QObject>
#include <QPluginLoader>
#include <QSettings>
#include <QStringList>
#include <QToolBar>
#include <QUrl>

#ifdef RWS_USE_PYTHON
#include <rws/pythonpluginloader/PyPlugin.hpp>
#endif    // RWS_USE_PYTHON
#include <RobWorkConfig.hpp>
#include <rws/propertyview/PropertyViewEditor.hpp>

#include <boost/bind/bind.hpp>
#include <boost/filesystem.hpp>
#include <sstream>

using namespace rw;
using namespace rw::core;
using namespace rw::common;
using namespace rw::loaders;
using namespace rw::math;
using namespace rw::models;
using namespace rw::kinematics;
using namespace rw::proximity;
using namespace rw::trajectory;
using namespace rwlibs::proximitystrategies;

using namespace rws;

namespace {
WorkCell::Ptr emptyWorkCell ()
{
    WorkCell::Ptr workcell = rw::core::ownedPtr (new WorkCell (ownedPtr (new StateStructure ())));
    CollisionSetup::set (CollisionSetup (), workcell);
    return workcell;
}

CollisionDetector::Ptr makeCollisionDetector (WorkCell::Ptr workcell)
{
    return rw::core::ownedPtr (new CollisionDetector (
        workcell, ProximityStrategyFactory::makeDefaultCollisionStrategy ()));
}
}    // namespace

RobWorkStudio::RobWorkStudio (const PropertyMap& map) :
    QMainWindow (NULL), _robwork (RobWork::getInstance ()), _aboutBox (NULL),
    _inStateUpdate (false), _settingsMap (NULL)
{
    this->setObjectName ("RobWorkStudio_MainWindow");
    // Always create the about box.
    _aboutBox = new AboutBox (RW_VERSION, RW_REVISION, this);
    // should load dynamically
    // rw::core::ExtensionRegistry::getInstance()->registerExtensions( ownedPtr( new
    // RWSImageLoaderPlugin() ) ); _robwork->getPluginRepository().addPlugin(ownedPtr( new
    // ColladaLoaderPlugin() ), true);
    std::stringstream sstr;
    sstr << " RobWorkStudio v" << RW_VERSION;
    QString qstr (sstr.str ().c_str ());
    setWindowTitle (qstr);

    // time 50ms
    setWindowIcon (QIcon (":/images/rw_logo_64x64.png"));
    boost::filesystem::path settingsPath ("rwsettings.xml");

    PropertyMap settings;
    if (exists (settingsPath)) {
        try {
            _propMap = DOMPropertyMapLoader::load ("rwsettings.xml");
        }
        catch (rw::core::Exception& e) {
            RW_WARN ("Could not load settings from 'rwsettings.xml': "
                     << e.getMessage ().getText () << "\n Using default settings!");
        }
        catch (std::exception& e) {
            RW_WARN ("Could not load settings from 'rwsettings.xml': "
                     << e.what () << "\n Using default settings!");
            // loading failed so we just go on with an empty map
        }
    }
    _propMap.set ("cmdline", map);
    PropertyMap* currentSettings = _propMap.getPtr< PropertyMap > ("RobWorkStudioSettings");
    if (currentSettings == NULL) {
        _propMap.add ("RobWorkStudioSettings", "Settings for RobWorkStudio", settings);
        currentSettings = _propMap.getPtr< PropertyMap > ("RobWorkStudioSettings");
    }

    _assistant   = new HelpAssistant ();
    _settingsMap = _propMap.getPtr< PropertyMap > ("RobWorkStudioSettings");

    // set the drag and drop property to true
    setupFileActions ();
    setupToolActions ();
    setupViewGL ();
    _propEditor = new PropertyViewEditor (NULL);
    _propEditor->setPropertyMap (&_propMap);
    setupPluginsMenu ();
    setupHelpMenu ();
    int width  = _settingsMap->get< int > ("WindowWidth", 1024);
    int height = _settingsMap->get< int > ("WindowHeight", 800);
    int x      = _settingsMap->get< int > ("WindowPosX", this->pos ().x ());
    int y      = _settingsMap->get< int > ("WindowPosY", this->pos ().y ());
    std::vector< int > state_vector =
        _settingsMap->get< std::vector< int > > ("QtMainWindowState", std::vector< int > ());
    if (state_vector.size () > 0) {
        QByteArray mainAppState ((int) state_vector.size (), 0);
        for (int i = 0; i < mainAppState.size (); i++) {
            mainAppState[i] = (char) state_vector[i];
        }
        this->restoreState (mainAppState);
    }
    resize (width, height);
    this->move (x, y);

    _workcell = emptyWorkCell ();
    _state    = _workcell->getDefaultState ();
    _detector = makeCollisionDetector (_workcell);
    // Workcell given to view.
    _view->setWorkCell (_workcell);
    _view->setState (_state);

    // Workcell sent to plugins.
    openAllPlugins ();
    // updateHandler();

    setAcceptDrops (true);
}

RobWorkStudio::~RobWorkStudio ()
{
    delete _assistant;
    delete _propEditor;
}

void RobWorkStudio::propertyChangedListener (PropertyBase* base)
{
    std::string id = base->getIdentifier ();
}

void RobWorkStudio::closeEvent (QCloseEvent* e)
{
    QByteArray mainAppState = saveState ();
    std::vector< int > state_vector (mainAppState.size ());
    for (int i = 0; i < mainAppState.size (); i++) {
        state_vector[i] = mainAppState[i];
    }

    _settingsMap->set< std::vector< int > > ("QtMainWindowState", state_vector);
    _settingsMap->set< int > ("WindowPosX", this->pos ().x ());
    _settingsMap->set< int > ("WindowPosY", this->pos ().y ());
    _settingsMap->set< int > ("WindowWidth", this->width ());
    _settingsMap->set< int > ("WindowHeight", this->height ());

    closeAllPlugins ();

    // close all plugins
    typedef std::vector< RobWorkStudioPlugin* >::iterator I;
    for (I it = _plugins.begin (); it != _plugins.end (); ++it) {
        (*it)->QWidget::close ();
    }

    if (!_propMap.get< PropertyMap > ("cmdline").has ("NoSave")) {
        _propMap.set ("cmdline", PropertyMap ());
        _propMap.erase ("LuaState");
        try {
            DOMPropertyMapSaver::save (_propMap, "rwsettings.xml");
        }
        catch (const rw::core::Exception& e) {
            RW_WARN ("Error saving settings file: " << e);
        }
        catch (...) {
            RW_WARN ("Error saving settings file due to unknown exception!");
        }
    }
    _propMap = PropertyMap ();
    _propEditor->close ();

    _view->clear ();
    _view->close ();

    // now call accept
    e->accept ();
}

rw::core::Log& RobWorkStudio::log ()
{
    return _robwork->getLog ();
}

rw::core::Log::Ptr RobWorkStudio::logPtr ()
{
    return _robwork->getLogPtr ();
}

void RobWorkStudio::updateLastFiles ()
{
    QMenu* filemenu                                                = _fileMenu;
    std::vector< std::pair< QAction*, std::string > >& fileactions = _lastFilesActions;
    std::vector< std::string > nfiles = _settingsMap->get< std::vector< std::string > > (
        "LastOpennedFiles", std::vector< std::string > ());
    // remove old actions
    for (size_t i = 0; i < fileactions.size (); i++) {
        filemenu->removeAction (fileactions[i].first);
    }
    fileactions.clear ();

    // sort nfiles such that multiples are left out
    std::vector< std::string > tmp;
    for (size_t i = 0; i < nfiles.size (); i++) {
        int idx   = (int) (nfiles.size () - 1 - i);
        bool skip = false;
        for (std::string& str : tmp) {
            if (str == nfiles[idx]) {
                skip = true;
                break;
            }
        }
        if (!skip)
            tmp.push_back (nfiles[idx]);
        if (tmp.size () > 10)
            break;
    }
    nfiles.resize (tmp.size ());

    // now add the new ones
    for (size_t i = 0; i < tmp.size (); i++) {
        nfiles[tmp.size () - 1 - i] = tmp[i];
        boost::filesystem::path p (tmp[i]);
        std::stringstream sstr;
        sstr << i << ": " << p.filename ();
        QAction* nAction = filemenu->addAction (sstr.str ().c_str ());

        connect (nAction, SIGNAL (triggered ()), this, SLOT (setCheckAction ()));
        filemenu->addAction (nAction);
        fileactions.push_back (std::make_pair (nAction, tmp[i]));
    }

    _settingsMap->set< std::vector< std::string > > ("LastOpennedFiles", nfiles);
}

void RobWorkStudio::setupFileActions ()
{
    QAction* newAction = new QAction (QIcon (":/images/new.png"), tr ("&New"), this);    // owned
    connect (newAction, SIGNAL (triggered ()), this, SLOT (newWorkCell ()));

    QAction* openAction =
        new QAction (QIcon (":/images/open.png"), tr ("&Open..."), this);    // owned
    connect (openAction, SIGNAL (triggered ()), this, SLOT (open ()));

    QAction* closeAction =
        new QAction (QIcon (":/images/close.png"), tr ("&Close"), this);    // owned
    connect (closeAction, SIGNAL (triggered ()), this, SLOT (onCloseWorkCell ()));

    QAction* saveAction = new QAction (QIcon (":/images/save.png"), tr ("&Save"), this);    // owned
    connect (saveAction, SIGNAL (triggered ()), this, SLOT (saveWorkCell ()));

    QAction* reloadAction =
        new QAction (QIcon (":/images/reload.png"), tr ("&Reload"), this);    // owned
    reloadAction->setShortcut (Qt::Key_F5);
    connect (reloadAction, SIGNAL (triggered ()), this, SLOT (reloadWorkCell ()));

    QAction* exitAction = new QAction (QIcon (), tr ("&Exit"), this);    // owned
    connect (exitAction, SIGNAL (triggered ()), this, SLOT (close ()));

    QToolBar* fileToolBar = addToolBar (tr ("File"));
    fileToolBar->setObjectName ("FileToolBar");
    fileToolBar->addAction (newAction);
    fileToolBar->addAction (openAction);
    fileToolBar->addAction (closeAction);
    fileToolBar->addAction (saveAction);
    fileToolBar->addAction (reloadAction);
    ////
    _fileMenu = menuBar ()->addMenu (tr ("&File"));
    _fileMenu->addAction (newAction);
    _fileMenu->addAction (openAction);
    _fileMenu->addAction (closeAction);
    _fileMenu->addAction (saveAction);
    _fileMenu->addAction (reloadAction);
    _fileMenu->addSeparator ();

    QAction* propertyAction = new QAction (tr ("&Preferences"), this);    // owned
    connect (propertyAction, SIGNAL (triggered ()), this, SLOT (showPropertyEditor ()));

    _fileMenu->addAction (propertyAction);

    _fileMenu->addSeparator ();

    _fileMenu->addAction (exitAction);

    _fileMenu->addSeparator ();
    updateLastFiles ();
}

void RobWorkStudio::setupToolActions ()
{
    QAction* printCollisionsAction =
        new QAction (QIcon (""), tr ("Print Colliding Frames"), this);    // owned
    connect (printCollisionsAction, SIGNAL (triggered ()), this, SLOT (printCollisions ()));

    _toolMenu = menuBar ()->addMenu (tr ("&Tools"));
    _toolMenu->addAction (printCollisionsAction);
}

void RobWorkStudio::printCollisions ()
{
    CollisionDetector::Ptr cd = getCollisionDetector ();
    CollisionDetector::QueryResult res;
    cd->inCollision (getState (), &res);
    if (res.collidingFrames.size () > 0) {
        for (const FramePair& pair : res.collidingFrames) {
            std::cout << "Colliding: " << pair.first->getName () << " -- "
                      << pair.second->getName () << std::endl;
            Log::infoLog () << "Colliding: " << pair.first->getName () << " -- "
                            << pair.second->getName () << std::endl;
        }
    }
}

void RobWorkStudio::setCheckAction ()
{
    QObject* obj = sender ();

    // check if any of the open last file actions where choosen
    for (size_t i = 0; i < _lastFilesActions.size (); i++) {
        if (obj == _lastFilesActions[i].first) {
            openFile (_lastFilesActions[i].second);
            break;
        }
    }
}

void RobWorkStudio::showPropertyEditor ()
{
    // start property editor
    _propEditor->update ();
    _propEditor->show ();
    _propEditor->resize (400, 600);
}

void RobWorkStudio::setupPluginsMenu (bool create)
{
    QAction* loadPluginAction = new QAction (QIcon (""), tr ("Load plugin"), this);
    connect (loadPluginAction, SIGNAL (triggered ()), this, SLOT (loadPlugin ()));

    QAction* removePluginAction = new QAction (QIcon (""), tr ("Unload plugin"), this);
    connect (removePluginAction, SIGNAL (triggered ()), this, SLOT (unloadPlugin ()));

    if (_pluginsMenu == nullptr) {
        create = true;
    }

    if (create) {
        _pluginsMenu = menuBar ()->addMenu (tr ("&Plugins"));
    }
    else {
        _pluginsMenu->clear ();
    }
    _pluginsMenu->addAction (loadPluginAction);
    _pluginsMenu->addAction (removePluginAction);
    _pluginsMenu->addSeparator ();

    if (create) {
        _pluginsToolBar = addToolBar (tr ("Plugins"));
        _pluginsToolBar->setObjectName ("PluginsBar");
    }
    else {
        _pluginsToolBar->clear ();
    }
}

void RobWorkStudio::loadPlugin (std::string pluginFile, bool visible, int dock)
{
    if (boost::filesystem::exists (pluginFile)) {
        setupPlugin (pluginFile.c_str (), visible, dock);
    }
}

void RobWorkStudio::loadPlugin ()
{
    QString selectedFilter;

    std::string previousOpenDirectory =
        _settingsMap->get< std::string > ("PreviousOpenDirectory", "");
    const QString dir (previousOpenDirectory.c_str ());

    QString pluginfilename =
        QFileDialog::getOpenFileName (this,
                                      "Open plugin file",    // Title
                                      dir,                   // Directory
                                      "Plugin libraries ( *.so *.dll *.dylib *.so.* *.py)"
                                      "\n All ( *.* )",
                                      &selectedFilter);

    if (!pluginfilename.isEmpty ()) {
        QFileInfo pluginInfo (pluginfilename);
        QString pathname = pluginInfo.absolutePath ();
        QString filename = pluginInfo.baseName ();

        setupPlugin (pathname, filename, 0, 1);
    }
}

void RobWorkStudio::unloadPlugin ()
{
    QStringList list;
    for (RobWorkStudioPlugin* pl : _plugins) {
        list.append (pl->name ());
    }

    bool ok;
    QString text = QInputDialog::getItem (
        this, tr ("Unload plugin"), tr ("Which Plugin should be removed"), list, 0, false, &ok);

    if (ok) {
        std::cout << "OK: " << text.toStdString () << std::endl;
        for (RobWorkStudioPlugin* pl : _plugins) {
            if (pl->name () == text) {
                bool test = unloadPlugin (pl);
                std::cout << "test: " << test << std::endl;
                break;
            }
        }
    }
}

bool RobWorkStudio::unloadPlugin (RobWorkStudioPlugin* pl)
{
    removeDockWidget (pl);
    setupPluginsMenu (false);
    int remove = -1;
    for (size_t i = 0; i < _plugins.size (); i++) {
        if (_plugins[i] == pl) {
            remove = i;
        }
        else {
            _plugins[i]->setupMenu (_pluginsMenu);
            _plugins[i]->setupToolBar (_pluginsToolBar);
        }
    }
    if (remove < 0) {
        return false;
    }
    _plugins.erase (_plugins.begin () + remove);
    _plugins_loaded[_plugin2fileName[pl->name ().toStdString ()]] = false;
    return true;
}

void RobWorkStudio::setupHelpMenu ()
{
    QAction* assistantAct = new QAction (tr ("Help Contents"), this);
    assistantAct->setShortcut (QKeySequence::HelpContents);
    connect (assistantAct, SIGNAL (triggered ()), this, SLOT (showDocumentation ()));

    QAction* showAboutBox = new QAction ("About", this);
    connect (showAboutBox, SIGNAL (triggered ()), this, SLOT (showAboutBox ()));

    QMenu* pHelpMenu = menuBar ()->addMenu (tr ("Help"));
    pHelpMenu->addAction (assistantAct);
    pHelpMenu->addAction (showAboutBox);
}

void RobWorkStudio::keyPressEvent (QKeyEvent* e)
{
    keyEvent ().fire (e->key (), e->modifiers ());
    QWidget::keyPressEvent (e);
}

void RobWorkStudio::showAboutBox ()
{
    _aboutBox->exec ();
}

void RobWorkStudio::showDocumentation ()
{
    QStringList filepaths;
    // std::cout << QCoreApplication::applicationFilePath().toStdString() << std::endl;
    filepaths.append (QCoreApplication::applicationDirPath ());

    _assistant->showDocumentation (filepaths);
}

void RobWorkStudio::setupViewGL ()
{
    _view = new RWStudioView3D (this, this);
    setCentralWidget (_view);    // own view
    _view->setupGUI (this);
}

void RobWorkStudio::openAllPlugins ()
{
    typedef std::vector< RobWorkStudioPlugin* >::iterator I;
    for (I p = _plugins.begin (); p != _plugins.end (); ++p) {
        // RW_WARN( (*p)->name().toStdString() << "4");
        openPlugin (**p);
    }
}

void RobWorkStudio::closeAllPlugins ()
{
    typedef std::vector< RobWorkStudioPlugin* >::iterator PI;
    for (PI p = _plugins.begin (); p != _plugins.end (); ++p)
        closePlugin (**p);
}

void RobWorkStudio::openPlugin (RobWorkStudioPlugin& plugin)
{
    RW_ASSERT (_workcell);
    try {
        plugin.open (_workcell.get ());
    }
    catch (rw::core::Exception& exc) {
        std::stringstream buf;
        buf << "Exception in opening of plugin "
            << StringUtil::quote (plugin.name ().toStdString ());
#if !defined(RW_MACOS)
        QMessageBox::information (
            NULL, buf.str ().c_str (), exc.getMessage ().getText ().c_str (), QMessageBox::Ok);
#else
        this->log ().info () << buf.str () << std::endl
                             << " With message: " << exc.getMessage ().getText () << std::endl;
#endif
    }
    catch (...) {
        std::stringstream buf;
        buf << "Exception in opening of plugin "
            << StringUtil::quote (plugin.name ().toStdString ());

#if !defined(RW_MACOS)
        QMessageBox::information (NULL, buf.str ().c_str (), "Unknown error", QMessageBox::Ok);
#else
        this->log ().info () << buf.str () << std::endl << " With message: " << std::endl;
#endif
    }
}

void RobWorkStudio::closePlugin (RobWorkStudioPlugin& plugin)
{
    try {
        plugin.close ();
    }
    catch (const Exception& exc) {
        std::stringstream buf;
        buf << "Exception in closing of plugin "
            << StringUtil::quote (plugin.name ().toStdString ());

        QMessageBox::information (
            NULL, buf.str ().c_str (), exc.getMessage ().getText ().c_str (), QMessageBox::Ok);
    }
}

void RobWorkStudio::addPlugin (RobWorkStudioPlugin* plugin, bool visible, Qt::DockWidgetArea area)
{
    plugin->setLog (_robwork->getLogPtr ());
    plugin->setRobWorkStudio (this);
    plugin->setRobWorkInstance (_robwork);
    plugin->setupMenu (_pluginsMenu);
    plugin->setupToolBar (_pluginsToolBar);

    plugin->initialize ();

    // The updateSignal does not EXIST on the plugin interface....
    // connect(plugin, SIGNAL(updateSignal()), this, SLOT(updateHandler()));

    _plugins.push_back (plugin);
    std::string pname = plugin->name ().toStdString ();
    bool isVisible    = _settingsMap->get< bool > (std::string ("PluginVisible_") + pname, visible);
    bool isFloating   = _settingsMap->get< bool > (std::string ("PluginFloating_") + pname, false);
    int intarea       = _settingsMap->get< int > (std::string ("PluginArea_") + pname, (int) area);

    addDockWidget ((Qt::DockWidgetArea) intarea, plugin);
    // addDockWidget(area, plugin);
    plugin->setFloating (isFloating);
    // IMPORTANT visibility must be set as the last thing....
    plugin->setVisible (isVisible);
    // Only open the plugin if the work cell is loaded.
    if (_workcell)
        openPlugin (*plugin);

    std::vector< int > state_vector =
        _settingsMap->get< std::vector< int > > ("QtMainWindowState", std::vector< int > ());
    if (state_vector.size () > 0) {
        QByteArray mainAppState ((int) state_vector.size (), 0);
        for (int i = 0; i < mainAppState.size (); i++) {
            mainAppState[i] = (char) state_vector[i];
        }
        this->restoreState (mainAppState);
    }
}

void RobWorkStudio::loadSettingsSetupPlugins (const std::string& file)
{
    QSettings settings (file.c_str (), QSettings::IniFormat);
    switch (settings.status ()) {
        case QSettings::NoError: setupPlugins (settings); break;

        case QSettings::FormatError: {
            std::string msg = file + " file not loaded";
            QMessageBox::information (NULL, "Format error", msg.c_str (), QMessageBox::Ok);
        } break;

        case QSettings::AccessError:
            // Nothing to report here.
            break;
    }

    // TODO: make error reply if necessary
    // return settings.status();
}

void RobWorkStudio::setupPlugin (const QString& pathname, const QString& filename, bool visible,
                                 int dock)
{
    QString pfilename = pathname + "/" + filename + "." + OS::getDLLExtension ().c_str ();
    bool e1           = boost::filesystem::exists (pfilename.toStdString ());
    bool py           = false;
    if (!e1) {
        pfilename = pathname + "/" + filename;
        e1        = boost::filesystem::exists (pfilename.toStdString ());
    }
    if (!e1) {
        pfilename = pathname + "/" + filename + ".so";
        e1        = boost::filesystem::exists (pfilename.toStdString ());
    }
    if (!e1) {
        pfilename = pathname + "/" + filename + ".dll";
        e1        = boost::filesystem::exists (pfilename.toStdString ());
    }
    if (!e1) {
        pfilename = pathname + "/" + filename + ".dylib";
        e1        = boost::filesystem::exists (pfilename.toStdString ());
    }
    if (!e1) {
        pfilename = pathname + "/" + filename + ".py";
        e1        = boost::filesystem::exists (pfilename.toStdString ());
        py        = e1;
    }

    if (_plugins_loaded[filename.toStdString ()]) {
        RW_THROW ("Plugin \"" << filename.toStdString () << "\" has already been loaded");
    }

    if (py) {
        setupPyPlugin (pfilename, filename, visible, dock);
    }
    else {
        setupPlugin (pfilename, visible, dock);
    }
}

void RobWorkStudio::setupPlugin (const QString& fullname, bool visible, int dock)
{
    std::string ext  = boost::filesystem::extension (fullname.toStdString ());
    std::string base = boost::filesystem::basename (fullname.toStdString ());
    if (ext == "py" || ext == ".py") {
        setupPyPlugin (fullname, base.c_str (), visible, dock);
        return;
    }
    else if (!_plugins_loaded[base]) {
        Qt::DockWidgetArea dockarea = (Qt::DockWidgetArea) dock;
        QPluginLoader loader (fullname);

        // Needed to make dynamicly loaded libraries use dynamic
        // cast on each others objects. ONLY on linux though.
        loader.setLoadHints (QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);

        QObject* pluginObject = loader.instance ();
        if (pluginObject != NULL) {
            RobWorkStudioPlugin* testP = dynamic_cast< RobWorkStudioPlugin* > (pluginObject);
            if (testP == NULL) {
                RW_THROW ("Loaded plugin is NULL, tried loading \"" << fullname.toStdString ()
                                                                    << "\"");
            }
            RobWorkStudioPlugin* plugin = qobject_cast< RobWorkStudioPlugin* > (pluginObject);
            _plugin2fileName[plugin->name ().toStdString ()] = base;
            if (plugin) {
                addPlugin (plugin, visible, dockarea);
            }
            else {
                RW_WARN ("Unable to load Plugin" << fullname.toStdString ()
                                                 << " was not of type RobWorkStudioPlugin");
                QMessageBox::information (this,
                                          "Unable to load Plugin",
                                          fullname + " was not of type RobWorkStudioPlugin",
                                          QMessageBox::Ok);
            }
        }
        else {
            RW_WARN ("Unable to load Plugin" << fullname.toStdString () << " was not loaded: \""
                                             << loader.errorString ().toStdString () + "\"");
            QMessageBox::information (this,
                                      "Unable to load Plugin",
                                      fullname + " was not loaded: \"" + loader.errorString () +
                                          "\"",
                                      QMessageBox::Ok);
        }
        _plugins_loaded[base] = true;
    }
}

void RobWorkStudio::setupPyPlugin (const QString& pathname, const QString& filename, bool visible,
                                   int dock)
{
#ifdef RWS_USE_PYTHON
    Qt::DockWidgetArea dockarea = (Qt::DockWidgetArea) dock;
    PyPlugin* pyplug            = new PyPlugin (filename, QIcon (":/PythonIcon.png"));
    addPlugin (pyplug, visible, dockarea);
    pyplug->initialize (pathname.toLocal8Bit ().data (), filename.toLocal8Bit ().data ());
#endif    // RWS_USE_PYTHON
#ifndef RWS_USE_PYTHON
    RW_THROW ("You have attempted to load a python plugin, but RobWorkStudio was not compiled "
              "using python, please recompile RobWorkStudio");
#endif    // NOT RWS_USE_PYTHON
}

void RobWorkStudio::setupPlugins (QSettings& settings)
{
    QStringList groups = settings.childGroups ();

    settings.beginGroup ("Plugins");
    QStringList plugins = settings.childGroups ();
    for (int i = 0; i < plugins.size (); i++) {
        const QString& pluginname = plugins.at (i);
        Log::debugLog () << "Plugin = " << pluginname.toStdString () << "\n";

        settings.beginGroup (pluginname);

        QString pathname = settings.value ("Path").toString ();
        QString filename = settings.value ("Filename").toString ();
        bool visible     = settings.value ("Visible").toBool ();
        // Qt::DockWidgetArea dockarea =
        //    (Qt::DockWidgetArea)settings.value("DockArea").toInt();
        int dock = settings.value ("DockArea").toInt ();

        setupPlugin (pathname, filename, visible, dock);
        settings.endGroup ();    // End Specific Plugin Group
    }
    settings.endGroup ();    // End the Plugins Group
}

std::string RobWorkStudio::loadSettingsWorkcell (const std::string& file)
{
    QSettings settings (file.c_str (), QSettings::IniFormat);
    std::string workcellPath = "";
    switch (settings.status ()) {
        case QSettings::NoError: {
            QStringList groups = settings.childGroups ();
            settings.beginGroup ("Settings");
            QStringList theSettings = settings.childGroups ();

            for (int i = 0; i < theSettings.size (); i++) {
                const QString& settingName = theSettings.at (i);
                Log::debugLog () << "SettingFound: " << settingName.toStdString () << "\n";

                if (settingName.toStdString () == "Workcell") {
                    settings.beginGroup ("Workcell");
                    workcellPath = settings.value ("Path").toString ().toStdString ();
                    break;
                }
            }

            break;
        }
        case QSettings::FormatError:
        case QSettings::AccessError:
            // Nothing to report here.
            break;
    }
    Log::debugLog () << "workcellFound: " << workcellPath << "\n";
    return workcellPath;
}

void RobWorkStudio::newWorkCell ()
{
    try {
        closeWorkCell ();
        // Empty workcell constructed.
        _workcell = emptyWorkCell ();
        _state    = _workcell->getDefaultState ();
        _detector = makeCollisionDetector (_workcell);
        // Workcell given to view.
        _view->setWorkCell (_workcell);
        _view->setState (_state);
    }
    catch (const Exception& exp) {
        QMessageBox::critical (
            this,
            tr ("RobWorkStudio"),
            tr ("Caught exception while trying to create new work cell: %1").arg (exp.what ()));
    }

    // Workcell sent to plugins.
    openAllPlugins ();
    updateHandler ();
}

void RobWorkStudio::saveWorkCell ()
{
    if (_workcell != nullptr) {
        std::string wcFilePath =
            static_cast< std::string > (_workcell->getPropertyMap ().get< std::string > (
                rw::loaders::XMLRWLoader::getWorkCellFileNameId ()));
        QString wcFileName = QFileDialog::getSaveFileName (this,
                                                           tr ("Save Workcell"),
                                                           QString::fromStdString (wcFilePath),
                                                           tr ("RobWork Workcell (*.wc.xml)"));

        rw::loaders::DOMWorkCellSaver::save (_workcell, _state, wcFileName.toStdString ());
    }
}

void RobWorkStudio::reloadWorkCell ()
{
    try {
        if (_workcell->getFilename ().size () > 1) {
            Log::infoLog () << "reloading: " << _workcell->getFilename () << "\n";
            openWorkCellFile (QString (_workcell->getFilename ().c_str ()));
        }
    }
    catch (const rw::core::Exception& exp) {
        QMessageBox::information (
            NULL, "Exception", exp.getMessage ().getText ().c_str (), QMessageBox::Ok);
    }
}

void RobWorkStudio::dragMoveEvent (QDragMoveEvent* event)
{
    event->accept ();
}

void RobWorkStudio::dragEnterEvent (QDragEnterEvent* event)
{
    if (event->mimeData ()->hasText ()) {
        event->acceptProposedAction ();
    }
    else if (event->mimeData ()->hasHtml ()) {
        event->acceptProposedAction ();
    }
    else if (event->mimeData ()->hasUrls ()) {
        event->acceptProposedAction ();
    }
    else {
        event->ignore ();
    }
}

void RobWorkStudio::dropEvent (QDropEvent* event)
{
    if (event->mimeData ()->hasUrls ()) {
        QList< QUrl > urls = event->mimeData ()->urls ();
        if (urls.size () == 1) {
            openFile (urls[0].toLocalFile ().toStdString ());
        }
    }
    else if (event->mimeData ()->hasHtml ()) {
        Log::debugLog () << "html dropped: " << std::endl;
    }
    else if (event->mimeData ()->hasText ()) {
        QString text = event->mimeData ()->text ();
        Log::infoLog () << text.toStdString () << std::endl;
    }
    else {
        event->ignore ();
    }
}

void RobWorkStudio::openFile (const std::string& file)
{
    // We change directory irrespective of whether we can load open the file or
    // not. We change directory also if openFile() was called because of a drop
    // event.
    if (!file.empty ()) {
        _settingsMap->set< std::string > ("PreviousOpenDirectory",
                                          StringUtil::getDirectoryName (file));
    }

    try {
        const QString filename (file.c_str ());
        // std::cout << filename.toStdString() << std::endl;

        if (!filename.isEmpty ()) {
            std::vector< std::string > lastfiles = _settingsMap->get< std::vector< std::string > > (
                "LastOpennedFiles", std::vector< std::string > ());
            lastfiles.push_back (file);
            if (filename.endsWith (".STL", Qt::CaseInsensitive) ||
                filename.endsWith (".STLA", Qt::CaseInsensitive) ||
                filename.endsWith (".STLB", Qt::CaseInsensitive) ||
#if RW_HAVE_ASSIMP
                filename.endsWith (".DAE", Qt::CaseInsensitive) ||
#endif
                filename.endsWith (".3DS", Qt::CaseInsensitive) ||
                filename.endsWith (".AC", Qt::CaseInsensitive) ||
                filename.endsWith (".AC3D", Qt::CaseInsensitive) ||
                filename.endsWith (".TRI", Qt::CaseInsensitive) ||
                filename.endsWith (".OBJ", Qt::CaseInsensitive)) {
                Log::infoLog () << "Opening drawable file: " << filename.toStdString () << "\n";
                openDrawable (filename);
                _settingsMap->set< std::vector< std::string > > ("LastOpennedFiles", lastfiles);
                updateLastFiles ();
            }
            else if (filename.endsWith (".WC", Qt::CaseInsensitive) ||
                     filename.endsWith (".XML", Qt::CaseInsensitive)) {
                Log::infoLog () << "Opening workcell file: " << filename.toStdString () << "\n";
                openWorkCellFile (filename);
                _settingsMap->set< std::vector< std::string > > ("LastOpennedFiles", lastfiles);
                updateLastFiles ();
            }
            else if (filename.endsWith (".rwplay", Qt::CaseInsensitive) |
                     filename.endsWith (".csv", Qt::CaseInsensitive)) {
                // Log::infoLog() << "The RobWorkStudio::OpenFile() function can't load playback
                // files\n";
                RW_THROW ("The RobWorkStudio::OpenFile() function can't load playback files");
            }
            else {
                // we try openning a workcell
                openWorkCellFile (filename);
            }
        }
    }
    catch (const rw::core::Exception& exp) {
        QMessageBox::information (
            NULL, "Exception", exp.getMessage ().getText ().c_str (), QMessageBox::Ok);
    }
    // std::cout << "Update handler!" << std::endl;
    updateHandler ();
}

void RobWorkStudio::open ()
{
    QString selectedFilter;

    std::string previousOpenDirectory =
        _settingsMap->get< std::string > ("PreviousOpenDirectory", "");
    const QString dir (previousOpenDirectory.c_str ());

    QString assimpExtensions = "";
#if RW_HAVE_ASSIMP
    assimpExtensions = " * .dae";
#endif

    QString filename = QFileDialog::getOpenFileName (
        this,
        "Open WorkCell or Drawable",    // Title
        dir,                            // Directory
        "All supported ( *.wu *.wc *.xml *.wc.xml *.dev *.stl *.stla *.stlb *.3ds *.ac *.ac3d "
        "*.obj" +
            assimpExtensions +
            ")"
            "\nRW XML files ( *.wc.xml *.xml *.wc)"
            "\nDrawables ( *.stl *.stla *.stlb *.3ds *.ac *.ac3d *.obj" +
            assimpExtensions +
            ")"
            "\n All ( *.* )",
        &selectedFilter);

    openFile (filename.toStdString ());
}

void RobWorkStudio::openDrawable (const QString& filename)
{
    try {
        _view->getWorkCellScene ()->addDrawable (filename.toStdString (),
                                                 _workcell->getWorldFrame ());
    }
    catch (...) {
        const std::string msg = "Failed to load " + filename.toStdString () + " as a Drawable";
        QMessageBox::information (this, "Error", msg.c_str (), QMessageBox::Ok);
    }
}

void RobWorkStudio::openWorkCellFile (const QString& filename)
{
    // Always close the workcell.
    closeWorkCell ();
    // rw::graphics::WorkCellScene::Ptr wcsene = _view->makeWorkCellScene();

    WorkCell::Ptr wc;

    try {
        wc = WorkCellLoader::Factory::load (filename.toStdString ());
        if (wc == NULL) {
            RW_THROW ("Loading of workcell failed!");
        }
    }
    catch (const std::exception& e) {
        const std::string msg = "Failed to load workcell: " + filename.toStdString () + ". \n " +
                                std::string (e.what ());
        QMessageBox::information (this, "Error", msg.c_str (), QMessageBox::Ok);
        wc = emptyWorkCell ();
    }

    // std::cout<<"Number of devices in workcell in RobWorkStudio::setWorkCell:
    // "<<workcell->getDevices().size()<<std::endl;
    // don't set any variables before we know they are good

    CollisionDetector::Ptr detector = makeCollisionDetector (wc);
    _workcell                       = wc;
    _state                          = _workcell->getDefaultState ();
    _detector                       = detector;
    _view->setWorkCell (wc);
    _view->setState (_state);

    openAllPlugins ();
}

void RobWorkStudio::setWorkcell (rw::models::WorkCell::Ptr workcell)
{
    // Always close the workcell.
    if (_workcell && workcell != _workcell) {
        closeWorkCell ();
    }

    // Open a new workcell if there is one.<
    if (workcell && workcell != _workcell) {
        // don't set any variables before we know they are good
        CollisionDetector::Ptr detector = makeCollisionDetector (workcell);
        _workcell                       = workcell;
        _state                          = _workcell->getDefaultState ();
        _detector                       = detector;
        _view->setWorkCell (_workcell);
        _view->setState (_state);
        openAllPlugins ();

        double scale = this->calculateWorkCellSize ().diagonal ().norm2 ();
        // set maximum zoom scale at 2m and minimum at 20cm
        scale = std::min (std::min (0.1, scale / 2.0), 1.0);
        if (_propMap.has ("ZoomScale")) {
            _propMap.set ("ZoomScale", scale);
        }
        else {
            _propMap.add ("ZoomScale","value [0-1] scaling the zoom factor of the cameracontroller", scale);
        }
    }
}

rw::models::WorkCell::Ptr RobWorkStudio::getWorkcell ()
{
    return _workcell;
}

void RobWorkStudio::closeWorkCell ()
{
    _workcell = nullptr;
    _detector = nullptr;
    _state    = State ();
    // Clear everything from the view
    _view->clear ();

    // Call close on all modules
    closeAllPlugins ();

    updateHandler ();
}

void RobWorkStudio::showSolidTriggered ()
{
    updateHandler ();
}

void RobWorkStudio::showWireTriggered ()
{
    updateHandler ();
}

void RobWorkStudio::showBothTriggered ()
{
    updateHandler ();
}

void RobWorkStudio::updateViewHandler ()
{
    _view->update ();
}

void RobWorkStudio::updateHandler ()
{
    update ();
    _view->update ();
}

void RobWorkStudio::setTStatePath (rw::trajectory::TimedStatePath path)
{
    _timedStatePath = ownedPtr (new rw::trajectory::TimedStatePath (path));
    stateTrajectoryChangedEvent ().fire (*_timedStatePath);
    stateTrajectoryPtrChangedEvent ().fire (_timedStatePath);
}

namespace {

class RobWorkStudioEvent : public QEvent
{
  public:
    static const QEvent::Type SetStateEvent          = (QEvent::Type) 1200;
    static const QEvent::Type SetTimedStatePathEvent = (QEvent::Type) 1201;
    static const QEvent::Type UpdateAndRepaintEvent  = (QEvent::Type) 1202;
    static const QEvent::Type SaveViewGLEvent        = (QEvent::Type) 1203;
    static const QEvent::Type ExitEvent              = (QEvent::Type) 1204;
    static const QEvent::Type SetWorkCell            = (QEvent::Type) 1205;
    static const QEvent::Type OpenWorkCell           = (QEvent::Type) 1206;
    static const QEvent::Type CloseWorkCell          = (QEvent::Type) 1207;
    static const QEvent::Type GenericEvent           = (QEvent::Type) 1208;
    static const QEvent::Type GenericAnyEvent        = (QEvent::Type) 1209;

    boost::any _anyData;
    rw::core::Ptr< bool > _hs;

    RobWorkStudioEvent (QEvent::Type type, rw::core::Ptr< bool > hs, boost::any adata) :
        QEvent (type), _anyData (adata), _hs (hs)
    {}

    RobWorkStudioEvent (QEvent::Type type, rw::core::Ptr< bool > hs) :
        QEvent (type), _anyData (NULL), _hs (hs)
    {}

    virtual ~RobWorkStudioEvent ()
    {
        Log::debugLog () << "RobWorkStudioEvent: destruct" << std::endl;
        done ();
    }

    void done ()
    {
        if (_hs != NULL) {
            Log::debugLog () << "Done: " << std::endl;
            Log::debugLog () << "set hs " << std::endl;
            *_hs = true;
            Log::debugLog () << "hs " << *_hs << std::endl;
        }
        else {
            Log::debugLog () << "Done: hs==NULL" << std::endl;
        }
    }
};

class RobWorkStudioEventHS
{
  public:
    RobWorkStudioEventHS (QEvent::Type type, boost::any adata) : _hs (ownedPtr (new bool (false)))
    {
        event = new RobWorkStudioEvent (type, _hs, adata);
    }

    RobWorkStudioEventHS (QEvent::Type type) : _hs (ownedPtr (new bool (false)))
    {
        event = new RobWorkStudioEvent (type, _hs);
    }

    void wait ()
    {
        int cnt = 0;
        while (_hs != NULL && *_hs == false && cnt < 100) {
            TimerUtil::sleepMs (5);
            cnt++;
        }
    }

    rw::core::Ptr< bool > _hs;
    RobWorkStudioEvent* event;
};

}    // namespace

void RobWorkStudio::setTimedStatePath (const rw::trajectory::TimedStatePath& path)
{
    _timedStatePath = ownedPtr (new rw::trajectory::TimedStatePath (path));
    stateTrajectoryChangedEvent ().fire (*_timedStatePath);
    stateTrajectoryPtrChangedEvent ().fire (_timedStatePath);
}

void RobWorkStudio::setTimedStatePath (const rw::trajectory::TimedStatePath::Ptr path)
{
    _timedStatePath = path;
    stateTrajectoryChangedEvent ().fire (*_timedStatePath);
    stateTrajectoryPtrChangedEvent ().fire (_timedStatePath);
}

void RobWorkStudio::setState (const rw::kinematics::State& state)
{
    _state = state;
    _view->setState (state);
    updateHandler ();
    stateChangedEvent ().fire (_state);
}

void RobWorkStudio::postTimedStatePath (const rw::trajectory::TimedStatePath& path)
{
    RobWorkStudioEventHS* event =
        new RobWorkStudioEventHS (RobWorkStudioEvent::SetTimedStatePathEvent, path);
    QApplication::postEvent (this, event->event);
    event->wait ();
}

void RobWorkStudio::postExit ()
{
    RobWorkStudioEventHS* event = new RobWorkStudioEventHS (RobWorkStudioEvent::ExitEvent, NULL);
    QApplication::postEvent (this, event->event);
    // event->wait();
}

void RobWorkStudio::postState (const rw::kinematics::State& state)
{
    RobWorkStudioEventHS* event =
        new RobWorkStudioEventHS (RobWorkStudioEvent::SetStateEvent, state);
    QApplication::postEvent (this, event->event);
    event->wait ();
}

void RobWorkStudio::postUpdateAndRepaint ()
{
    RobWorkStudioEventHS* event =
        new RobWorkStudioEventHS (RobWorkStudioEvent::UpdateAndRepaintEvent);
    QApplication::postEvent (this, event->event);
    event->wait ();
}

void RobWorkStudio::postSaveViewGL (const std::string& filename)
{
    RobWorkStudioEventHS* event =
        new RobWorkStudioEventHS (RobWorkStudioEvent::SaveViewGLEvent, filename);
    QApplication::postEvent (this, event->event);
    event->wait ();
}

void RobWorkStudio::postWorkCell (rw::models::WorkCell::Ptr workcell)
{
    RobWorkStudioEventHS* event =
        new RobWorkStudioEventHS (RobWorkStudioEvent::SetWorkCell, workcell);
    QApplication::postEvent (this, event->event);
    event->wait ();
}

void RobWorkStudio::postOpenWorkCell (const std::string& filename)
{
    RobWorkStudioEventHS* event =
        new RobWorkStudioEventHS (RobWorkStudioEvent::OpenWorkCell, filename);
    QApplication::postEvent (this, event->event);
    event->wait ();
}

void RobWorkStudio::postCloseWorkCell ()
{
    RobWorkStudioEventHS* event =
        new RobWorkStudioEventHS (RobWorkStudioEvent::CloseWorkCell, NULL);
    QApplication::postEvent (this, event->event);
    event->wait ();
}

void RobWorkStudio::postGenericEvent (const std::string& id)
{
    RobWorkStudioEventHS* event = new RobWorkStudioEventHS (RobWorkStudioEvent::GenericEvent, id);
    QApplication::postEvent (this, event->event);
    event->wait ();
}

void RobWorkStudio::postGenericAnyEvent (const std::string& id, boost::any data)
{
    RobWorkStudioEventHS* event =
        new RobWorkStudioEventHS (RobWorkStudioEvent::GenericAnyEvent, std::make_pair (id, data));
    QApplication::postEvent (this, event->event);
    event->wait ();
}

bool RobWorkStudio::event (QEvent* event)
{
    // WARNING: only use this pointer if you know its the right type
    RobWorkStudioEvent* rwse = static_cast< RobWorkStudioEvent* > (event);
    if (event->type () == RobWorkStudioEvent::SetStateEvent) {
        State state = boost::any_cast< State > (rwse->_anyData);
        setState (state);
        rwse->done ();
        return true;
    }
    else if (event->type () == RobWorkStudioEvent::SetTimedStatePathEvent) {
        TimedStatePath tstate = boost::any_cast< TimedStatePath > (rwse->_anyData);
        setTimedStatePath (tstate);
        rwse->done ();
        return true;
    }
    else if (event->type () == RobWorkStudioEvent::UpdateAndRepaintEvent) {
        updateAndRepaint ();
        rwse->done ();
        return true;
    }
    else if (event->type () == RobWorkStudioEvent::SaveViewGLEvent) {
        std::string str = boost::any_cast< std::string > (rwse->_anyData);
        try {
            saveViewGL (QString (str.c_str ()));
        }
        catch (const Exception& exp) {
            QMessageBox::critical (
                NULL,
                "Save View",
                tr ("Failed to grab and save view with message '%1'").arg (exp.what ()));
        }
        catch (std::exception& e) {
            QMessageBox::critical (
                NULL,
                "Save View",
                tr ("Failed to grab and save view with message '%1'").arg (e.what ()));
        }
        catch (...) {
            QMessageBox::critical (NULL, "Save View", tr ("Failed to grab and save view"));
        }
        rwse->done ();
        return true;
    }
    else if (event->type () == RobWorkStudioEvent::SetWorkCell) {
        WorkCell::Ptr wc = boost::any_cast< WorkCell::Ptr > (rwse->_anyData);
        setWorkCell (wc);
        rwse->done ();
        return true;
    }
    else if (event->type () == RobWorkStudioEvent::OpenWorkCell) {
        std::string str = boost::any_cast< std::string > (rwse->_anyData);
        openWorkCellFile (str.c_str ());
        rwse->done ();
        return true;
    }
    else if (event->type () == RobWorkStudioEvent::CloseWorkCell) {
        onCloseWorkCell ();
        rwse->done ();
        return true;
    }
    else if (event->type () == RobWorkStudioEvent::GenericEvent) {
        std::string id = boost::any_cast< std::string > (rwse->_anyData);
        this->genericEvent ().fire (id);
        rwse->done ();
        return true;
    }
    else if (event->type () == RobWorkStudioEvent::GenericAnyEvent) {
        std::pair< std::string, boost::any > data =
            boost::any_cast< std::pair< std::string, boost::any > > (rwse->_anyData);
        this->genericAnyEvent ().fire (data.first, data.second);
        rwse->done ();
        return true;
    }
    else if (event->type () == RobWorkStudioEvent::ExitEvent) {
        onCloseWorkCell ();
        rwse->done ();
        close ();
        return true;
    }
    else if (event->type () == QEvent::Close) {
        QApplication::closeAllWindows ();
    }
    else {
        // event->ignore();
    }

    return QMainWindow::event (event);
}

void RobWorkStudio::saveViewGL (const QString& filename)
{
    _view->saveBufferToFile (filename);
}
namespace {
class AnyEventListener
{
  public:
    AnyEventListener (const std::string& myid) : _id (myid), _eventSuccess (false) {}
    void cb (const std::string& id, boost::any data)
    {
        // std::cout << "Any event recieved in CALLBACK!!!!" << std::endl;
        if (!_eventSuccess) {
            if (_id == id) {
                _data         = data;
                _eventSuccess = true;
            }
        }
    }
    std::string _id;
    boost::any _data;
    bool _eventSuccess;
};
}    // namespace

boost::any RobWorkStudio::waitForAnyEvent (const std::string& id, double timeout)
{
    // std::cout << " Wait for ANY event, with id: " << id << std::endl;
    AnyEventListener listener (id);
    genericAnyEvent ().add (
        boost::bind (&AnyEventListener::cb, &listener, boost::arg< 1 > (), boost::arg< 2 > ()),
        &listener);
    // std::cout << "Added event, now wait!" << std::endl;
    // now wait until event is called
    const double starttime = TimerUtil::currentTime ();
    bool reachedTimeout    = false;
    while (!listener._eventSuccess) {
        TimerUtil::sleepMs (10);
        if ((timeout > 0.0) && (TimerUtil::currentTime () - starttime > timeout)) {
            reachedTimeout = true;
            break;
        }
    }
    // remove the listener from the event
    genericAnyEvent ().remove (&listener);
    // now return result
    if (reachedTimeout)
        RW_THROW ("Timeout!");
    return listener._data;
}

rw::geometry::AABB< double > RobWorkStudio::calculateWorkCellSize ()
{
    std::vector< rw::geometry::BSphere< double > > spheres;
    std::vector< Object::Ptr > objects = this->_workcell->getObjects ();
    State& state                       = this->_state;
    for (Object::Ptr object : objects) {
        for (rw::geometry::Geometry::Ptr geom : object->getGeometry (state)) {
            rw::core::Ptr< Frame > frame = geom->getFrame ();
            RW_ASSERT (frame);
            spheres.push_back (
                rw::geometry::BSphere< double >::fitEigen (geom->getGeometryData ()));
            spheres.back ().setPosition (spheres.back ().getPosition () + frame->wTf (state).P ());
        }
    }

    std::vector< Vector3D< double > > points;
    for (rw::geometry::BSphere< double >& s : spheres) {
        double r = s.getRadius ();
        points.push_back (s.getPosition () + Vector3D< double > (r, 0, 0));
        points.push_back (s.getPosition () + Vector3D< double > (-r, 0, 0));
        points.push_back (s.getPosition () + Vector3D< double > (0, r, 0));
        points.push_back (s.getPosition () + Vector3D< double > (0, -r, 0));
        points.push_back (s.getPosition () + Vector3D< double > (0, 0, r));
        points.push_back (s.getPosition () + Vector3D< double > (0, 0, -r));
    }

    Vector3D< double > axis_max (-99999, -99999, -999999);
    Vector3D< double > axis_min (99999, 999999, 999999);
    for (Vector3D< double >& p : points) {
        for (size_t i = 0; i < p.size (); i++) {
            if (p[i] > axis_max[i]) {
                axis_max[i] = p[i];
            }
            if (p[i] < axis_min[i]) {
                axis_min[i] = p[i];
            }
        }
    }
    return rw::geometry::AABB< double > (axis_min, axis_max);
}