
#include <memory>

#include "Firestore/core/src/model/document.h"

class Document {
 public:
  explicit Document(MutableDocument&& document)
      : document_(std::move(document)) {
  }
  const MutableDocument& get() const {
    return document_;
  }
  const MutableDocument* operator->() const {
    return get();
  }

 private:
  MutableDocument document_;
};