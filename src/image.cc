#include <string>

#include "image.h"
#include "utils.h"
#include "worker.h"

NAN_MODULE_INIT(Image::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);

  tpl->SetClassName(Nan::New("Image").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "process", Process);
  Nan::SetPrototypeMethod(tpl, "copy", Copy);
  Nan::SetPrototypeMethod(tpl, "size", Size);

  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());

  Nan::Set(target, Nan::New("Image").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

Image::Image() {}
Image::~Image() {}

Nan::Persistent<v8::Function> & Image::constructor() {
  static Nan::Persistent<v8::Function> my_constructor;
  return my_constructor;
}

NAN_METHOD(Image::New) {
  if (info.IsConstructCall()) {
    Image *obj = new Image();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    return Nan::ThrowError("'Image' is a class not a function. Use 'new' operator to instantiate it.");
  }
}

NAN_METHOD(Image::Copy) {
  Image *image = Nan::ObjectWrap::Unwrap<Image>(info.Holder());

  v8::Local<v8::Function> cons = Nan::New(constructor());
  Nan::MaybeLocal<v8::Object> maybeInstance = Nan::NewInstance(cons, 0, NULL);
  Image *newImage = Nan::ObjectWrap::Unwrap<Image>(
    maybeInstance.ToLocalChecked()
  );
  newImage->image = image->image;

  info.GetReturnValue().Set(maybeInstance.ToLocalChecked());
}

NAN_METHOD(Image::Size) {
  Image *image = Nan::ObjectWrap::Unwrap<Image>(info.Holder());
  Magick::Geometry size;

  size_t width = Nan::To<uint32_t>(info[0]).FromJust();
  size_t height = Nan::To<uint32_t>(info[1]).FromJust();
  if (width > 0 && height > 0) {
    size.width(width);
    size.height(height);
    image->image.size(size);
  } else {
    size = image->image.size();
  }

  v8::Local<v8::Object> response = Nan::New<v8::Array>();
  Nan::Set(
    response, 
    Nan::New("width").ToLocalChecked(),
    Nan::New<v8::Uint32>((uint32_t) size.width())
  );
  Nan::Set(
    response,
    Nan::New("height").ToLocalChecked(),
    Nan::New<v8::Uint32>((uint32_t) size.height())
  );

  info.GetReturnValue().Set(response);
}

NAN_METHOD(Image::Process) {
  Nan::MaybeLocal<v8::Object> maybeOptions = Nan::To<v8::Object>(info[0]);
  if (maybeOptions.IsEmpty()) {
    return Nan::ThrowError("'options' parameter is not provided");
  }
  Nan::MaybeLocal<v8::Function> maybeCallback = Nan::To<v8::Function>(info[1]);
  if (maybeCallback.IsEmpty()) {
    return Nan::ThrowError("'callback' parameter is not provided");
  }

  Image *image = Nan::ObjectWrap::Unwrap<Image>(info.Holder());
  v8::Local<v8::Object> options = maybeOptions.ToLocalChecked();
  v8::Local<v8::Object> task = Nan::New<v8::Object>();

  // input
  if (HasAttr(options, "input")) {
    v8::Local<v8::Object> input = GetAttrLocal<v8::Object>(options, "input");
    if (HasAttr(input, "buffer")) {
      Nan::Set(
        task,
        Nan::New("inputBuffer").ToLocalChecked(),
        GetAttrLocal<v8::Object>(input, "buffer")
      );
    } else if (HasAttr(input, "file")) {
      Nan::Set(
        task,
        Nan::New("inputFile").ToLocalChecked(),
        GetAttrLocal<v8::String>(input, "file")
      );
    }
  }

  // output
  if (HasAttr(options, "output")) {
    v8::Local<v8::Object> output = GetAttrLocal<v8::Object>(options, "output");
    if (HasAttr(output, "buffer")) {
      Nan::Set(
        task,
        Nan::New("outputToBuffer").ToLocalChecked(),
        GetAttrLocal<v8::Boolean>(output, "buffer")
      );
    }

    if (HasAttr(output, "file")) {
      Nan::Set(
        task,
        Nan::New("outputFile").ToLocalChecked(),
        GetAttrLocal<v8::String>(output, "file")
      );
    }
  }

  if (HasAttr(options, "autorotate")) {
    Nan::Set(
      task,
      Nan::New("autorotate").ToLocalChecked(),
      GetAttrLocal<v8::Boolean>(options, "autorotate")
    );
  }

  if (HasAttr(options, "rotate")) {
    Nan::Set(
      task,
      Nan::New("rotate").ToLocalChecked(),
      GetAttrLocal<v8::Number>(options, "rotate")
    );
  }

  if (HasAttr(options, "strip")) {
    Nan::Set(
      task,
      Nan::New("strip").ToLocalChecked(),
      GetAttrLocal<v8::Boolean>(options, "strip")
    );
  }

  if (HasAttr(options, "crop")) {
    Nan::Set(
      task,
      Nan::New("crop").ToLocalChecked(),
      GetAttrLocal<v8::String>(options, "crop")
    );
  }

  if (HasAttr(options, "resize")) {
    Nan::Set(
      task,
      Nan::New("resize").ToLocalChecked(),
      GetAttrLocal<v8::String>(options, "resize")
    );
  }

  if (HasAttr(options, "extent")) {
    Nan::Set(
      task,
      Nan::New("extent").ToLocalChecked(),
      GetAttrLocal<v8::String>(options, "extent")
    );
  }

  Nan::Callback *callback = new Nan::Callback(maybeCallback.ToLocalChecked());
  Nan::AsyncQueueWorker(new ImageWorker(callback, image, task));

  info.GetReturnValue().Set(info.This());
}
