#include "worker.h"
#include "image.h"
#include "utils.h"

ImageWorker::ImageWorker(
  Nan::Callback *callback,
  Image *image,
  v8::Local<v8::Object> &task
): AsyncWorker(callback, "ImageWorker"), image(image) {
  if (HasAttr(task, "input")) {
    v8::Local<v8::Object> inputData = GetAttrLocal<v8::Object>(task, "input");

    input = new InputParams();
    if (HasAttr(inputData, "buffer")) {
      input->fromBuffer = true;
      v8::Local<v8::Object> buffer = GetAttrLocal<v8::Object>(inputData, "buffer");
      image->blob.update(
        node::Buffer::Data(buffer),
        node::Buffer::Length(buffer)
      );
    }
    if (HasAttr(inputData, "file")) {
      input->file = GetAttrStr(inputData, "file");
    }
  }

  if (HasAttr(task, "output")) {
    v8::Local<v8::Object> outputData = GetAttrLocal<v8::Object>(task, "output");

    output = new OutputParams();
    if (HasAttr(outputData, "buffer")) {
      output->toBuffer = GetAttrJust<bool>(outputData, "buffer");
    }
    if (HasAttr(outputData, "file")) {
      output->file = GetAttrStr(outputData, "file");
    }
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

  if (HasAttr(task, "extent")) {
    v8::Local<v8::Object> extentData = GetAttrLocal<v8::Object>(task, "extent");

    extent = new ExtentParams();
    extent->size = GetAttrStr(extentData, "size");
    if (HasAttr(extentData, "color")) {
      extent->color = GetAttrStr(extentData, "color");
    }
    if (HasAttr(extentData, "gravity")) {
      extent->gravity = (Magick::GravityType) GetAttrJust<uint32_t>(extentData, "gravity");
    }
  }
}

ImageWorker::~ImageWorker() {
  delete input;
  delete output;
  delete extent;
}

void ImageWorker::Execute() {
  try {
    // input
    if (input != nullptr) {
      if (input->fromBuffer) {
        image->image.read(image->blob);
      } else if (!input->file.empty()) {
        image->image.read(input->file);
      }
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

    // extent
    if (extent != nullptr) {
      if (!extent->color.empty()) {
        image->image.extent(extent->size, extent->color, extent->gravity);
      } else {
        image->image.extent(extent->size, extent->gravity);
      }
    }

    // strip
    if (strip) {
      image->image.strip();
    }

    // output
    if (output != nullptr) {
      if (output->toBuffer) {
        image->image.write(&output->blob);
      } else if (!output->file.empty()) {
        image->image.write(output->file);
      }
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

  if (output != nullptr) {
    if (output->toBuffer) {
      char *blobData = (char *) malloc(output->blob.length());
      memcpy(blobData, output->blob.data(), output->blob.length());
      argv[1] = Nan::NewBuffer(
        blobData,
        output->blob.length(),
        FreeDataCallback,
        nullptr
      ).ToLocalChecked();
    } else if (!output->file.empty()) {
      argv[1] = Nan::New<v8::String>(output->file).ToLocalChecked();
    }
  }

  callback->Call(2, argv, async_resource);
}
