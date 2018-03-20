#include <nan.h>

#include "image.h"

NAN_MODULE_INIT(Init) {
  Image::Init(target);
}

NODE_MODULE(addon, Init)
