
#include "RWSImageLoaderPlugin.hpp"

#include "ImageUtil.hpp"

#include <rw/core/Ptr.hpp>
#include <rw/loaders/ImageLoader.hpp>

#include <QImage>
#include <QImageReader>

using namespace rws;
using namespace rw::sensor;
using namespace rw::core;

RW_ADD_PLUGIN (RWSImageLoaderPlugin)

namespace {

class QImageLoader : public rw::loaders::ImageLoader
{
  public:
    virtual ~QImageLoader () {}

    std::vector< std::string > getImageFormats ()
    {
        Log::debugLog () << "Image Formats: \n";
        QList< QByteArray > formats = QImageReader::supportedImageFormats ();
        std::vector< std::string > subformats;
        for (QByteArray& format : formats) {   
            std::string str = format.toUpper ().data ();
            Log::debugLog () << "  " << str << "\n";
            subformats.push_back (str);
        }

        return subformats;
    }

    rw::sensor::Image::Ptr loadImage (const std::string& filename)
    {
        // load the image
        QImage img;
        if (!img.load (filename.c_str ())) {
            RW_WARN ("Cannot load image: " << filename);
            return NULL;
        }
        // std::cout << "image size: " <<  img.width() << " " << img.height() << std::endl;
        // std::cout << "filename: " << filename << std::endl;
        // convert to robwork Image
        Image::Ptr rwImg = ImageUtil::toRwImage (img);
        // rwImg->saveAsPPM(filename + ".ppm");
        return rwImg;
    }
};

}    // namespace

RWSImageLoaderPlugin::RWSImageLoaderPlugin () :
    Plugin ("RWSImageLoaderPlugin", "RWSImageLoaderPlugin", "0.1")
{}

RWSImageLoaderPlugin::~RWSImageLoaderPlugin ()
{}

std::vector< Extension::Descriptor > RWSImageLoaderPlugin::getExtensionDescriptors ()
{
    std::vector< Extension::Descriptor > exts;
    exts.push_back (Extension::Descriptor ("QImageLoader", "rw.loaders.ImageLoader"));
    // QList<QByteArray> formats = QImageReader::supportedImageFormats();
    std::string formats[] = {"BMP",
                             "GIF",
                             "JPG",
                             "JPEG",
                             "PNG",
                             "PBM",
                             "PGM",
                             "PPM",
                             "XBM",
                             "XPM",
                             "SVG"};
    
    // for(QByteArray& format: formats){
    for (std::string& format : formats) {
        // std::cout << "setting format: " << format.toUpper().data() << std::endl;
        exts.back ().getProperties ().set (format, true);
    }
    return exts;
}

rw::core::Ptr< Extension > RWSImageLoaderPlugin::makeExtension (const std::string& str)
{
    if (str == "QImageLoader") {
        Extension::Ptr extension    = rw::core::ownedPtr (new Extension (
            "QImageLoader", "rw.loaders.ImageLoader", this, ownedPtr (new QImageLoader ())));
        QList< QByteArray > formats = QImageReader::supportedImageFormats ();
        for (QByteArray& format : formats) {
            extension->getProperties ().set (format.toUpper ().data (), true);
        }
        return extension;
    }
    return NULL;
}
