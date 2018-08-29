/* Emulate "Synchronized" keyword in Java,
 *
 *     1. SYNCHRONIZED
 *         SYNCHRONIZED {
 *             // ...
 *         }
 *
 *     2. SYNCHRONIZED_OBJ(obj)
 *         1)  SYNCHRONIZED_OBJ(t, &T::getMutex_f) {
 *                 // ...
 *             }
 *         2)  void A::foo(){
 *                 SYNCHRONIZED_OBJ(*this, &T::getMutex_f) {
 *                     // ...
 *                 }
 *             }
 *
 *     3. SYNCHRONIZED_METHOD
 *         void foo() SYNCHRONIZED_METHOD {
 *             // ...
 *         }
 *
 *     4. SYNCHRONIZED_MEMBER_FUNCTION
 *         void A::foo() SYNCHRONIZED_MEMBER_FUNCTION(Mutex) {
 *             // ...
 *         }
 */
#ifndef _SYNCHRONIZED_HPP
#define _SYNCHRONIZED_HPP
#include <mutex>

#define INJECT_VARIABLE(type, name) \
	if (bool obfuscatedName = false) ; else \
		for (type name; !obfuscatedName; obfuscatedName = true)

	/*
	 * SYNCHRONIZED
	 */
#define SYNCHRONIZED \
	INJECT_VARIABLE(static std::mutex, obfuscatedMutexName) \
	INJECT_VARIABLE(std::lock_guard<std::mutex>, obfuscatedLockName (obfuscatedMutexName))

	/*
	 * SYNCHRONIZED_OBJ
	 */
#define SYNCHRONIZED_OBJ(obj, getMutex_f) \
	INJECT_VARIABLE(std::lock_guard<std::remove_reference_t<decltype((obj.*getMutex_f)())>>,            \
						obfuscatedLockName((obj.*getMutex_f)()))

	/*
	 * SYNCHRONIZED_METHOD
	 */
#define SYNCHRONIZED_METHOD \
	try { \
		INJECT_VARIABLE(static std::mutex, obfuscatedMutexName) \
		throw std::move(std::unique_lock<std::mutex>(obfuscatedMutexName)); \
	} catch (std::unique_lock<std::mutex>&)

    /*
     * SYNCHRONIZED_MEMBER_FUNCTION
     */
#define SYNCHRONIZED_MEMBER_FUNCTION(Mutex) \
	try { \
		throw std::move(std::unique_lock<std::remove_reference_t<decltype(Mutex)>>(Mutex)); \
	} catch (std::unique_lock<std::mutex>&)

#endif // !_SYNCHRONIZED_HPP
