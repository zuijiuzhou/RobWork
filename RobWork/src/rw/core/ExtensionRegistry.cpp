#include "ExtensionRegistry.hpp"

#include <rw/core/Plugin.hpp>
#include <rw/core/RobWork.hpp>

using namespace rw::core;

rw::core::Ptr< ExtensionRegistry > ExtensionRegistry::getInstance ()
{
    return rw::core::RobWork::getInstance ()->getExtensionRegistry ();
}

ExtensionRegistry::ExtensionRegistry ()
{
    // load settings file
}

ExtensionRegistry::~ExtensionRegistry ()
{
    clearExtensions ();
}

std::vector< rw::core::Extension::Descriptor >
ExtensionRegistry::getExtensionDescriptors (const std::string& ext_point_id) const
{
    std::vector< Extension::Descriptor > result;

    std::map< std::string,
              std::vector< std::pair< Extension::Descriptor, rw::core::Ptr< Plugin > > > >::
        const_iterator it;
    it = _descMap.find (ext_point_id);
    if (it == _descMap.end ())
        return std::vector< Extension::Descriptor > ();

    typedef std::pair< Extension::Descriptor, rw::core::Ptr< Plugin > > Desc;
    for (const Desc& desc : it->second) {
        result.push_back (desc.first);
    }
    return result;
}

std::vector< rw::core::Ptr< Extension > >
ExtensionRegistry::getExtensions (const std::string& ext_point_id) const
{
    std::vector< rw::core::Ptr< Extension > > result;

    std::map< std::string,
              std::vector< std::pair< Extension::Descriptor, rw::core::Ptr< Plugin > > > >::
        const_iterator it;
    it = _descMap.find (ext_point_id);
    if (it == _descMap.end ())
        return result;

    typedef std::pair< Extension::Descriptor, rw::core::Ptr< Plugin > > Desc;
    for (const Desc& desc : it->second) {
        result.push_back (desc.second->makeExtension (desc.first.id));
    }
    return result;
}

std::vector< rw::core::Ptr< Plugin > > ExtensionRegistry::getPlugins () const
{
    std::vector< rw::core::Ptr< Plugin > > result;

    for (const rw::core::Ptr< Plugin >& plugin : _plugins) {
        result.push_back (plugin);
    }

    return result;
}

void ExtensionRegistry::registerExtensions (rw::core::Ptr< Plugin > plugin)
{
    // make sure plugins with same id does not add duplicates of extension points
    for (Extension::Descriptor desc : plugin->getExtensionDescriptors ()) {
        if (_descMap.find (desc.point) != _descMap.end ()) {
            // check if plugin/extension allready exists
            std::vector< std::pair< Extension::Descriptor, rw::core::Ptr< Plugin > > >& res =
                _descMap[desc.point];
            for (size_t i = 0; i < res.size (); i++) {
                if (res[i].first.id == desc.id && res[i].second->getId () == plugin->getId ()) {
                    continue;
                }
            }
        }
        _descMap[desc.point].push_back (std::make_pair (desc, plugin));
    }
    _plugins.push_back (plugin);
}

void ExtensionRegistry::unregisterExtensions (rw::core::Ptr< Plugin > plugin)
{
    for (Extension::Descriptor desc : plugin->getExtensionDescriptors ()) {
        if (_descMap.find (desc.point) != _descMap.end ()) {
            std::vector< std::pair< Extension::Descriptor, rw::core::Ptr< Plugin > > >& res =
                _descMap[desc.point];
            std::vector< std::pair< Extension::Descriptor, rw::core::Ptr< Plugin > > >::iterator
                it = res.begin ();
            while (it != res.end ()) {
                if (it->first.id == desc.id && it->second->getId () == plugin->getId ()) {
                    it = res.erase (it);
                }
                else {
                    it++;
                }
            }
        }
    }
    std::list< rw::core::Ptr< Plugin > >::iterator it = _plugins.begin ();
    while (*it != plugin && it != _plugins.end ()) {
        if (*it == plugin)
            it = _plugins.erase (it);
        else
            it++;
    }
}

void ExtensionRegistry::clearExtensions ()
{
    _descMap.clear ();
    // Remove plugins in reverse order as they were added.
    while (_plugins.size () > 0) {
        _plugins.erase (--_plugins.end ());
    }
}
