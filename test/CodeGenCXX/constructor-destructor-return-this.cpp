//RUN: %clang_cc1 %s -emit-llvm -o - -triple=thumbv7-apple-ios3.0 -target-abi apcs-gnu | FileCheck %s

// For constructors/desctructors that return 'this', if there exists a callsite
// that returns 'this' and is immediately before the return instruction, make
// sure we are using the return value from the callsite.
// rdar://12818789

// CHECK: define linkonce_odr [[A:%.*]] @_ZN11ObjectCacheC1Ev([[A]] %this) unnamed_addr
// CHECK: {{%.*}} = call [[A]] @_ZN11ObjectCacheC2Ev(

// CHECK: define linkonce_odr [[A:%.*]] @_ZN5TimerI11ObjectCacheEC1EPS0_MS0_FvPS1_E([[A]] %this
// CHECK: {{%.*}} = call [[A]] @_ZN5TimerI11ObjectCacheEC2EPS0_MS0_FvPS1_E(

// CHECK: define linkonce_odr [[A:%.*]] @_ZN5TimerI11ObjectCacheED1Ev([[A]] %this) unnamed_addr
// CHECK: {{%.*}} = call [[A]] @_ZN5TimerI11ObjectCacheED2Ev(

// CHECK: define linkonce_odr [[A:%.*]] @_ZN5TimerI11ObjectCacheED2Ev([[A]] %this) unnamed_addr
// CHECK: {{%.*}} = call [[B:%.*]] @_ZN9TimerBaseD2Ev(

class TimerBase {
public:
    TimerBase();
    virtual ~TimerBase();
};

template <typename TimerFiredClass> class Timer : public TimerBase {
public:
    typedef void (TimerFiredClass::*TimerFiredFunction)(Timer*);

    Timer(TimerFiredClass* o, TimerFiredFunction f)
        : m_object(o), m_function(f) { }

private:
    virtual void fired() { (m_object->*m_function)(this); }

    TimerFiredClass* m_object;
    TimerFiredFunction m_function;
};

class ObjectCache {
public:
    explicit ObjectCache();
    ~ObjectCache();

private:
    Timer<ObjectCache> m_notificationPostTimer;
};

inline ObjectCache::ObjectCache() : m_notificationPostTimer(this, 0) { }
inline ObjectCache::~ObjectCache() { }

ObjectCache *test() {
  ObjectCache *dd = new ObjectCache();
  return dd;
}
