#include "types.h"
#include "user.h"

int main() {
  schedlog(10000);
  int nicevalue = 0;
  for (int i = 0; i < 3; i++) {
    nicevalue = (i==0) ? -10 : (i == 1) ? 0 : 10;
    if (nicefork(nicevalue) == 0) {
      char *argv[] = {"loop", 0};
      exec("loop", argv);
    }
  }

  for (int i = 0; i < 3; i++) {
    wait();
  }
  
  shutdown();
}
