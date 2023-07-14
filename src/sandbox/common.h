#pragma once
#include <format>
#include <iostream>
#include <string>
#include <type_traits>
#define _COMM_BEGIN namespace comm{
#define _COMM_END }
#define _COMM ::comm::

_COMM_BEGIN

//拷贝构造设置
#define CLASS_CONSTRUCTOR_COPY(Class,Set)\
    Class(const Class& other) = Set;\
    Class(Class&& other) noexcept = Set;                    
//移动构造设置
#define CLASS_CONSTRUCTOR_MOVE(Class,Set)\
    Class& operator=(const Class & other) = Set;\
    Class& operator=(Class && other) noexcept = Set;  

#define CLASS_NO_COPY(Class) CLASS_CONSTRUCTOR_COPY(##Class, delete)
#define CLASS_NO_MOVE(Class) CLASS_CONSTRUCTOR_MOVE(##Class, delete)
#define CLASS_NO_COPY_AND_MOVE(Class)\
	CLASS_CONSTRUCTOR_COPY(##Class, delete)\
	CLASS_CONSTRUCTOR_MOVE(##Class, delete)

#define CLASS_DEFAULT_COPY(Class) CLASS_CONSTRUCTOR_COPY(##Class, default)
#define CLASS_DEFAULT_MOVE(Class) CLASS_CONSTRUCTOR_MOVE(##Class, default)
#define CLASS_DEFAULT_COPY_AND_MOVE(Class)\
	CLASS_CONSTRUCTOR_COPY(##Class, default)\
	CLASS_CONSTRUCTOR_MOVE(##Class, default)

#define CLASS_NO_COPY_DEFAULT_MOVE(Class)\
	CLASS_NO_COPY(Class)\
	CLASS_DEFAULT_MOVE(Class)

#define CLASS_DEFAULT_COPY_NO_MOVE(Class)\
	CLASS_DEFAULT_COPY(Class)\
	CLASS_NO_MOVE(Class)


#define VOID_POINTER(val)\
    reinterpret_cast<void*>(val)

constexpr std::string_view path_container_jpg("C:/Users/zhang/Pictures/Applications/OpenGLExample/container.jpg");
constexpr std::string_view path_awe_png("C:/Users/zhang/Pictures/Applications/OpenGLExample/awesomeface.png");
constexpr std::string_view dir_shader(R"(C:/WorkSpace/MyFile/MyCode/OpenGLExample/Shader)");
constexpr std::string_view dir_picture(R"(C:/Users/zhang/Pictures/Material)");
constexpr std::string_view path_materials(R"(C:/WorkSpace/MyFile/MyCode/OpenGLExample/Materials/materials.txt)");




template <class... Types>
void print_line(const std::string_view fmt, Types&&... args) { std::cout << std::vformat(fmt, std::make_format_args(args...)) << "\n"; }
template <class... Types>
void print(const std::string_view fmt, Types&&... args) { std::cout << std::vformat(fmt, std::make_format_args(args...)); }

template<typename T>
T& const_cast_ref(const T& arg)
{
    return *const_cast<T*>(&arg);
}

template <typename T>
auto getOrCreate() requires std::is_default_constructible_v<T>
{
    static std::shared_ptr<T> inst(new T); return inst;
}


unsigned int loadTexture(std::string_view path);
unsigned int loadTexture(std::string_view fileName, std::string_view directory);




_COMM_END

#ifndef Qt
#define _QT_BEGIN namespace Qt{
#define _QT_END }
#define _QT ::Qt::

inline _QT_BEGIN

#  define QT_WARNING_PUSH                       __pragma(warning(push))
#  define QT_WARNING_DISABLE_GCC(text)
#  define QT_WARNING_POP                        __pragma(warning(pop))

template <typename T> T* qGetPtrHelper(T* ptr) { return ptr; }
template <typename T> T* qGetPtrHelper(const std::shared_ptr<T>& ptr) { return ptr.get(); }
template <typename T> T* qGetPtrHelper(const std::unique_ptr<T>& ptr) { return ptr.get(); }

// The body must be a statement:
#define Q_CAST_IGNORE_ALIGN(body) QT_WARNING_PUSH QT_WARNING_DISABLE_GCC("-Wcast-align") body QT_WARNING_POP
#define Q_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() \
    { Q_CAST_IGNORE_ALIGN(return reinterpret_cast<Class##Private *>(qGetPtrHelper(d_ptr));) } \
    inline const Class##Private* d_func() const \
    { Q_CAST_IGNORE_ALIGN(return reinterpret_cast<const Class##Private *>(qGetPtrHelper(d_ptr));) } \
    friend class Class##Private;

#define Q_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() \
    { Q_CAST_IGNORE_ALIGN(return reinterpret_cast<Class##Private *>(qGetPtrHelper(Dptr));) } \
    inline const Class##Private* d_func() const \
    { Q_CAST_IGNORE_ALIGN(return reinterpret_cast<const Class##Private *>(qGetPtrHelper(Dptr));) } \
    friend class Class##Private;

#define Q_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
    friend class Class;

#define Q_D(Class) Class##Private * const d = d_func()
#define Q_Q(Class) Class * const q = q_func()


_QT_END

#endif
