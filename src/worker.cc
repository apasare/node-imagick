#include "worker.h"
#include "image.h"
#include "utils.h"

ImageWorker::ImageWorker(
  Nan::Callback *callback,
  Image *image,
  v8::Local<v8::Object> &task
): AsyncWorker(callback, "ImageWorker"), image(image) {
  if (HasAttr(task, "inputBuffer")) {
    v8::Local<v8::Object> buffer = GetAttrLocal<v8::Object>(task, "inputBuffer");
    image->blob.update(
      node::Buffer::Data(buffer),
      node::Buffer::Length(buffer)
    );
  } else if (HasAttr(task, "inputFile")) {
    inputFile = GetAttrStr(task, "inputFile");
  }

  if (HasAttr(task, "outputToBuffer")) {
    outputToBuffer = GetAttrJust<bool>(task, "outputToBuffer");
  }

  if (HasAttr(task, "outputFile")) {
    outputFile = GetAttrStr(task, "outputFile");
  }

  if (HasAttr(task, "autorotate")) {
    autorotate = GetAttrJust<bool>(task, "autorotate");
  }

  if (HasAttr(task, "rotate")) {
    rotate = GetAttrJust<double>(task, "rotate");
  }

  if (HasAttr(task, "strip")) {
    strip = GetAttrJust<bool>(task, "strip");
  }

  if (HasAttr(task, "resize")) {
    resize = GetAttrStr(task, "resize");
  }

  if (HasAttr(task, "crop")) {
    crop = GetAttrStr(task, "crop");
  }
}

ImageWorker::~ImageWorker() {}

void ImageWorker::Execute() {
  try {
    // input
    if (image->blob.length()) {
      image->image.read(image->blob);
    } else if (!inputFile.empty()) {
      image->image.read(inputFile);
    }

    // autorotate
    if (autorotate) {
      image->image.rotate(GetAutorotateAngle(image->image.orientation()));
      image->image.orientation(Magick::OrientationType::UndefinedOrientation);
    }

    // rotate
    if (rotate) {
      image->image.rotate(rotate);
    }

    // crop
    if (!crop.empty()) {
      image->image.crop(crop);
    }

    // resize
    if (!resize.empty()) {
      image->image.resize(resize);
    }

    // strip
    if (strip) {
      image->image.strip();
    }

    // output
    if (outputToBuffer) {
      image->image.write(&outputBlob);
    } else if (!outputFile.empty()) {
      image->image.write(outputFile);
    }
  } catch (Magick::Exception &error) {
    SetErrorMessage(error.what());
  }
}

void ImageWorker::HandleOKCallback() {
  Nan::HandleScope scope;

  v8::Local<v8::Value> argv[] = {
    Nan::Null(),
    Nan::True()
  };

  if (outputToBuffer) {
    char *blobData = (char *) malloc(outputBlob.length());
    memcpy(blobData, outputBlob.data(), outputBlob.length());
    argv[1] = Nan::NewBuffer(
      blobData,
      outputBlob.length(),
      FreeDataCallback,
      nullptr
    ).ToLocalChecked();
  } else if (!outputFile.empty()) {
    argv[1] = Nan::New<v8::String>(outputFile).ToLocalChecked();
  }

  callback->Call(2, argv, async_resource);
}
