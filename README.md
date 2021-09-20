# RwLock
This is a simple implementaion of a readers-writer lock in c++. RwLock is not fair and readers may starve writers.
```c
#include "rwlock.h

int main() {
    RwLock lock;
    {
        // Calling read will return a gurad that will release the lock when it goes out of scope.
        ReadGuard guard = lock.read();
    }
    {
        // Writing works the exact same way.
        WriteGuard guard = lock.write();
    }
}
```
