## emulating this usage of the synchronized
[An interesting application of INJECT_VARIABLE is to emulate Java's synchronized keyword](http://www.drdobbs.com/exception-safety-analysis/184401728)

      1. SYNCHRONIZED
          SYNCHRONIZED {
              // ...
          }
 
      2. SYNCHRONIZED_OBJ(obj)
          1)  SYNCHRONIZED_OBJ(t, &T::getMutex_f) {
                  // ...
              }
          2)  void A::foo(){
                  SYNCHRONIZED_OBJ(*this, &T::getMutex_f) {
                      // ...
                  }
              }
 
      3. SYNCHRONIZED_METHOD
          void foo() SYNCHRONIZED_METHOD {
              // ...
          }
 
      4. SYNCHRONIZED_MEMBER_FUNCTION
          void A::foo() SYNCHRONIZED_MEMBER_FUNCTION(&T::getMutex_f) {
              // ...
          }
