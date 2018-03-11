#include <nan.h>
// @todo delete this?
#include <Magick++.h>

#include "image.h"

NAN_MODULE_INIT(Init) {
  Image::Init(target);
}

NODE_MODULE(addon, Init)
