#include "stdafx.h"
#include "common.h"

#include <execution>

#include "stb_image.h"
#include "stb_image_resize.h"

#ifdef WIN32
#include <Windows.h>
#endif


std::map<std::string, unsigned int> s_textureMap;

// An output image with N components has the following components interleaved
// in this order in each pixel:
//
//     N=#comp     components
//       1           grey
//       2           grey, alpha
//       3           red, green, blue
//       4           red, green, blue, alpha
consteval GLenum stb_channel_to_enum(int channel)
{
	switch (channel)
	{
	case 1:return GL_RED;
	case 3:return GL_RGB;
	case 4:return GL_RGBA;
	default: assert(0);
	}
	return GL_RGB;
}
consteval GLenum stb_enum_to_channel(int channel)
{
	switch (channel)
	{
	case GL_RED:return 1;
	case GL_RGB:return 3;
	case GL_RGBA:return 4;
	default: return 0;
	}
}
consteval unsigned int highbit(unsigned int x) {
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x - (x >> 1);
}
std::shared_ptr<stbi_uc[]> comm::resize_image(const stbi_uc* source, int w, int h, int channels, int r_w)
{
	constexpr int resolution_max = 4096;

	//int w_ceil = std::bit_ceil(static_cast<unsigned>(w));
	r_w = r_w <= resolution_max ? r_w : resolution_max;
	const int r_h = static_cast<int>(h * static_cast<double>(r_w) / w);
	std::shared_ptr<stbi_uc[]> odata = std::make_shared<stbi_uc[]>(static_cast<std::size_t>(r_w) * r_h * channels);
	if (channels > 3)
		stbir_resize_uint8_srgb(source, w, h, 0, odata.get(), r_w, r_h, 0, channels, 3, 1);
	else
		stbir_resize_uint8(source, w, h, 0, odata.get(), r_w, r_h, 0, channels);
	return odata;
}
std::shared_ptr<stbi_uc[]> comm::resize_image(const stbi_uc* source, int w, int h, int channels, int& r_w, int& r_h)
{
	constexpr int resolution_max = 4096;

	int w_ceil = std::bit_ceil(static_cast<unsigned>(w));
	r_w = w_ceil <= resolution_max ? w_ceil : resolution_max;
	r_h = static_cast<unsigned>(h * static_cast<double>(r_w) / w);
	std::shared_ptr<stbi_uc[]> odata = std::make_shared<stbi_uc[]>(r_w * r_h * channels);
	if (channels > 3)
		stbir_resize_uint8_srgb(source, w, h, 0, odata.get(), r_w, r_h, 0, channels, 3, 1);
	else
		stbir_resize_uint8(source, w, h, 0, odata.get(), r_w, r_h, 0, channels);
	return odata;
}

unsigned comm::loadTexture(std::string_view path, bool b_flip_vertically)
{
	//stbi_set_flip_vertically_on_load(b_flip_vertically);
	if (s_textureMap.contains(path.data()))
		return s_textureMap.at(path.data());
	unsigned int textureID;
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.data(), &width, &height, &nrComponents, stb_enum_to_channel(GL_RGBA));

	if(data)
	{
		int rw, rh;
		auto rdata = resize_image(data,width,height, 4, rw, rh);
		const GLenum format = stb_channel_to_enum(stb_enum_to_channel(GL_RGBA));

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, rw, rh, 0, format, GL_UNSIGNED_BYTE, rdata.get());
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}
	stbi_image_free(data);
	s_textureMap.insert_or_assign(path.data(), textureID);
	return textureID;
}

std::vector<unsigned int> comm::loadTexture(const std::vector<std::string>& paths, bool b_flip_vertically)
{
	//stbi_set_flip_vertically_on_load(b_flip_vertically);
	std::vector<unsigned int> ids(paths.size(), 0);

	using input_type = std::tuple<int, std::string_view>;
	using output_type = std::tuple<int, std::shared_ptr<stbi_uc[]>, int, int>;

	std::vector<input_type> inputs(paths.size());
	std::vector<output_type> outputs(paths.size());

	


	auto loadFile = [](const input_type& s)
	{
		output_type result{ std::get<0>(s), nullptr, 0, 0};
		int width, height, nrComponents;
		unsigned char* data = stbi_load(std::get<1>(s).data(), &width, &height, &nrComponents, 4);
		if (!data)return result;

		std::get<1>(result) = resize_image(data, width, height, 4
			, std::get<2>(result), std::get<3>(result));
		stbi_image_free(data);

		return result;
	};
	for (int i = 0; i < paths.size(); ++i)
	{
		inputs[i] = {
			i,
			paths[i]
		};
		if (s_textureMap.contains(paths[i]))
			ids[i] = s_textureMap.at(paths[i]);
	}
	int count = 0;
	const auto lastTimePoint = std::chrono::high_resolution_clock::now();
	std::for_each(std::execution::par, inputs.begin(), inputs.end(), [&](const input_type& input)
		{
			if (const int n = std::get<0>(input); ids[n] != 0)
			{
				std::get<0>(outputs[n]) = n;
				return;
			}
			std::cout << std::format("[{:.1f}%][{}]Load {}\n", static_cast<double>(count++ * 100) / inputs.size(), std::get<0>(input), std::get<1>(input));
			const auto output = loadFile(input);
			outputs[std::get<0>(input)] = output;
		});
	std::cout << std::format("[{:.1f}%]Load completed! Cost: {} sec\n", 100.0, std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - lastTimePoint));

	count = 0;
	for(auto& output: outputs)
	{

		if (ids[std::get<0>(output)] != 0)continue;
		auto& textureID = ids[std::get<0>(output)];
		auto& [n, rdata, rw, rh] = output;

		// 根据加载的数据生成纹理
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rw, rh, 0, GL_RGBA, GL_UNSIGNED_BYTE, rdata.get());
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		std::cout << std::format("[{:.1f}%]生成纹理：{} Size:{{{}, {}}}\n", static_cast<double>(++count * 100) / inputs.size(), textureID, rw, rh);  // NOLINT(clang-diagnostic-invalid-source-encoding)
		s_textureMap.insert_or_assign(std::get<1>(inputs[n]).data(), textureID);
	}

	return ids;
}

std::string comm::WideCharToAnsi(const wchar_t* ws)
{
	const int length = WideCharToMultiByte(CP_ACP, 0, ws, -1, nullptr, 0, nullptr, nullptr);
	if (length == 0) {
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	std::string s; s.resize(length);
	if (!WideCharToMultiByte(CP_ACP, 0, ws, -1, s.data(), length, nullptr, nullptr)) {
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	return s;
}

std::wstring comm::AnsiToWideChar(const char* s)
{
	const int length = MultiByteToWideChar(CP_ACP, 0, s, -1, nullptr, 0);
	if (length == 0) {
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	std::wstring ws; ws.resize(length);
	if (!MultiByteToWideChar(CP_ACP, 0, s, -1, ws.data(), length)) {
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	return ws;
}

std::u8string comm::WideCharToUtf8(const wchar_t* ws)
{
	const int length = WideCharToMultiByte(CP_UTF8, 0, ws, -1, nullptr, 0, nullptr, nullptr);
	if (length == 0) {
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	std::u8string u8s; u8s.resize(length);
	if (!WideCharToMultiByte(CP_UTF8, 0, ws, -1, reinterpret_cast<char*>(u8s.data()), length, nullptr, nullptr)) {
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	return u8s;
}

std::wstring comm::Utf8ToWideChar(const char8_t* u8s)
{
	const int length = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(u8s), -1, nullptr, 0);
	if (length == 0) {
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	std::wstring ws; ws.resize(length);
	if (!MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(u8s), -1, ws.data(), length)) {
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	return ws;
}

std::u8string comm::AnsiToUtf8(const char* s)
{
	return comm::WideCharToUtf8(comm::AnsiToWideChar(s).c_str());
}

std::string comm::Utf8ToAnsi(const char8_t* u8s)
{
	return comm::WideCharToAnsi(comm::Utf8ToWideChar(u8s).c_str());
}

unsigned comm::loadTexture(std::string_view fileName, std::string_view directory, bool b_flip_vertically)
{
	return loadTexture(std::string(directory).append("/").append(fileName), b_flip_vertically);
}


void common_unit_testing()
{
	comm::println("common unit test begin!");
	const std::wstring input_w = L"成功！";
	const std::u8string input_u8 = comm::WideCharToUtf8(input_w.c_str());
	const std::string input = comm::WideCharToAnsi(input_w.c_str());
	
	comm::println("Ansi<->WideChar: {}", comm::WideCharToAnsi(comm::AnsiToWideChar(input.c_str()).c_str()));
	comm::println("Utf8<->WideChar: {}", comm::WideCharToAnsi(comm::Utf8ToWideChar(input_u8.c_str()).c_str()));
	comm::println("Utf8<->Ansi: {}", comm::Utf8ToAnsi(comm::AnsiToUtf8(input.c_str()).c_str()));

}