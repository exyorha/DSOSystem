#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdint.h>
#include <string.h>

int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <OUTPUT SOURCE FILE> <OUTPUT HEADER FILE> [RESOURCE NAME=RESOURCE FILE]..." << std::endl;
		return 1;
	}

	std::ofstream sourceStream;
	sourceStream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
	sourceStream.open(argv[1], std::ios::out | std::ios::trunc);

	std::ofstream headerStream;
	headerStream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
	headerStream.open(argv[2], std::ios::out | std::ios::trunc);

	std::string sourceFilename(argv[2]);
	auto nameStart = sourceFilename.find_last_of('/');
	if (nameStart == std::string::npos) {
		nameStart = 0;
	}
	else {
		nameStart++;
	}

	auto nameEnd = sourceFilename.find('.', nameStart);
	size_t nameLength;

	if (nameEnd == std::string::npos) {
		nameLength = nameEnd;
	}
	else {
		nameLength = nameEnd - nameStart;
	}

	auto headerTag = "resource_" + sourceFilename.substr(nameStart, nameLength);

	headerStream << "#ifndef " << headerTag << "\n"
		            "#define " << headerTag << "\n"
		            "\n"
		            "#if defined(__cplusplus)\n"
					"extern \"C\" {\n"
					"#endif\n"
					"\n";

	sourceStream << "#include <" << sourceFilename.substr(nameStart, std::string::npos) << ">\n"
					"\n";

	for (int index = 3; index < argc; index++) {
		std::string pair(argv[index]);
		std::string name, filename;

		auto delimiter = pair.find('=');
		if (delimiter == std::string::npos) {
			name = pair;
			filename = pair;
		}
		else {
			name = pair.substr(0, delimiter);
			filename = pair.substr(delimiter + 1);
		}

		std::ifstream file;
		file.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
		file.open(filename, std::ios::in | std::ios::binary);
		file.seekg(0, std::ios::end);

		auto length = static_cast<size_t>(file.tellg());

		file.seekg(0);

		headerStream << "extern const unsigned char " << name << "[" << length << "];\n";

		sourceStream << "alignas(16) const unsigned char " << name << "[" << length << "] = {\n";

		std::vector<unsigned char> data(length);
		file.read(reinterpret_cast<char *>(data.data()), data.size());

		for (size_t pos = 0; pos < length; pos += 16) {
			sourceStream << "  ";

			auto chunk = std::min<size_t>(length - pos, 16);

			for (size_t byte = 0; byte < chunk; byte++) {
				sourceStream << "0x";
				sourceStream.fill('0');
				sourceStream.width(2);
				sourceStream << std::hex << static_cast<unsigned int>(data[pos + byte]);
				sourceStream << ", ";
			}

			sourceStream << "\n";
		}

		sourceStream << "};\n"
			"\n";

	}

	headerStream << "#if defined(__cplusplus)\n"
		"}\n"
		"#endif\n"
		"\n"
		"#endif\n";

	return 0;
}