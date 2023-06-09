#include <imweb.hpp>
#include <iostream>

int main(int argc, char **argv) {
  ImWeb imweb;

  imweb.init(1024, 768);
  imweb.loop();

  return 0;
}
