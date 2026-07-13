// PR c++/125066
// { dg-do compile { target c++11 } }

class Callback;
struct Handler {
  Handler(Callback);
};
struct [[clang::trivial_abi]] Holder {
  ~Holder();
};
struct [[clang::trivial_abi]] Callback {
  Holder holder;
};
Handler::Handler(Callback) {}
