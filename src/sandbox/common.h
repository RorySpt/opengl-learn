#pragma once
#include <chrono>
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

#define CLASS_NO_COPY(Class) CLASS_CONSTRUCTOR_COPY(Class, delete)
#define CLASS_NO_MOVE(Class) CLASS_CONSTRUCTOR_MOVE(Class, delete)
#define CLASS_NO_COPY_AND_MOVE(Class)\
	CLASS_CONSTRUCTOR_COPY(Class, delete)\
	CLASS_CONSTRUCTOR_MOVE(Class, delete)

#define CLASS_DEFAULT_COPY(Class) CLASS_CONSTRUCTOR_COPY(Class, default)
#define CLASS_DEFAULT_MOVE(Class) CLASS_CONSTRUCTOR_MOVE(Class, default)
#define CLASS_DEFAULT_COPY_AND_MOVE(Class)\
	CLASS_CONSTRUCTOR_COPY(Class, default)\
	CLASS_CONSTRUCTOR_MOVE(Class, default)

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




//template <typename... Types>
//void println(const std::string_view fmt = "", Types&&... args) { std::cout << std::vformat(fmt, std::make_format_args(std::forward<Types>(args)...)) << "\n"; }
//template <typename... Types>
//void print(const std::string_view fmt, Types&&... args) { std::cout << std::vformat(fmt, std::make_format_args(std::forward<Types>(args)...)); }


inline std::mutex print_mutex;

template <int = 0>
void println(const std::string_view s = {})
{
    std::lock_guard guard(print_mutex);
    std::cout << s << '\n';
}

template<typename... Types>
concept PrintParameterPackConcept = sizeof... (Types) > 0;

constexpr bool b = PrintParameterPackConcept<int>;

template <PrintParameterPackConcept... Types> 
void println(const std::format_string<Types...> fmt, Types&&... args)
{
    std::lock_guard guard(print_mutex);
    std::cout << std::format(fmt, std::forward<Types>(args)...) << '\n';
}

template <int = 0>
void print(const std::string_view s)
{
    std::lock_guard guard(print_mutex);
    std::cout << s;
}
template <PrintParameterPackConcept... Types> 
void print(const std::format_string<Types...> fmt, Types&&... args)
{
    std::lock_guard guard(print_mutex);
    std::cout << std::format(fmt, std::forward<Types>(args)...);
}

template<typename T>
T& const_cast_ref(const T& arg)
{
    return *const_cast<T*>(&arg);
}

template <typename T>
auto getOrCreate() requires std::is_default_constructible_v<T>
{
    static std::shared_ptr<T> inst = std::make_shared<T>(); return inst;
}

std::shared_ptr<unsigned char[]> resize_image(const unsigned char* source, int w, int h, int channels, int r_w); // 缩放到指定宽度
std::shared_ptr<unsigned char[]> resize_image(const unsigned char* source, int w, int h, int channels, int& r_w, int& r_h);

unsigned int loadTexture(std::string_view path, bool b_flip_vertically = true);
unsigned int loadTexture(std::string_view fileName, std::string_view directory, bool b_flip_vertically = true);
std::vector<unsigned int> loadTexture(const std::vector<std::string>& paths, bool b_flip_vertically = true);

//template<typename T>
//concept ZonedTimeConcept = std::is_base_of<std::chrono::zoned_time<T>,>


inline std::string GetCurrentTimeString(const std::format_string<std::chrono::zoned_time<std::chrono::system_clock::duration>> fmt)
{
    return std::format(fmt, std::chrono::zoned_time{ std::chrono::current_zone(),
        std::chrono::system_clock::now() });
}
inline std::string GetCurrentTimeString()
{
    return GetCurrentTimeString("{:L%F %H:%M:%S}{}");
}

std::string WideCharToAnsi(const wchar_t* ws);
std::wstring AnsiToWideChar(const char* s);
std::u8string WideCharToUtf8(const wchar_t* ws);
std::wstring Utf8ToWideChar(const char8_t* u8s);
std::u8string AnsiToUtf8(const char* s);
std::string Utf8ToAnsi(const char8_t* u8s);

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
