#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <string>
#include <nan.h>
#include <Magick++.h>

void FreeDataCallback(char *data, void *hint);

bool HasAttr(v8::Local<v8::Object> object, std::string attribute);
bool HasAttr(Nan::MaybeLocal<v8::Object> object, std::string attribute);

Nan::MaybeLocal<v8::Value> GetAttr(v8::Local<v8::Object> object, std::string attribute);
Nan::MaybeLocal<v8::Value> GetAttr(Nan::MaybeLocal<v8::Object> object, std::string attribute);

template<typename T> Nan::MaybeLocal<T> GetAttrMaybeLocal(v8::Local<v8::Object> object, std::string attribute) {
  return Nan::To<T>(GetAttr(object, attribute).ToLocalChecked());
}

template<typename T> Nan::Maybe<T> GetAttrMaybe(v8::Local<v8::Object> object, std::string attribute) {
  return Nan::To<T>(GetAttr(object, attribute).ToLocalChecked());
}

template<typename T> v8::Local<T> GetAttrLocal(v8::Local<v8::Object> object, std::string attribute) {
  return GetAttrMaybeLocal<T>(object, attribute).ToLocalChecked();
}
template<typename T> v8::Local<T> GetAttrLocal(Nan::MaybeLocal<v8::Object> object, std::string attribute) {
  return GetAttrLocal<T>(object.ToLocalChecked(), attribute);
}

template<typename T> T GetAttrJust(v8::Local<v8::Object> object, std::string attribute) {
  return GetAttrMaybe<T>(object, attribute).FromJust();
}
template<typename T> T GetAttrJust(Nan::MaybeLocal<v8::Object> object, std::string attribute) {
  return GetAttrJust<T>(object.ToLocalChecked(), attribute);
}

std::string GetAttrStr(v8::Local<v8::Object> object, std::string attribute);

double GetAutorotateAngle(Magick::OrientationType orientationType);

#endif
