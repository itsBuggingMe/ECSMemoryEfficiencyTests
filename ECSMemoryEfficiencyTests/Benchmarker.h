#include <chrono>
#include <unordered_map>
#include <tuple>
#include <string>
#include <fstream>
#include <sstream>

#pragma once

class Benchmarker
{
private:
	using Key = std::tuple<std::string, std::string>;

	std::unordered_map<Key, int64_t> _entries;

	std::unordered_map<Key, std::chrono::high_resolution_clock::time_point> _active;

public:
	void Begin(const std::string& title, const std::string& category)
	{
		Key key{ title, category };
		_active[key] = std::chrono::high_resolution_clock::now();
	}

	void End(const std::string& title, const std::string& category)
	{
		auto end = std::chrono::high_resolution_clock::now();

		Key key{ title, category };

		auto it = _active.find(key);
		if (it == _active.end()) return;

		auto start = it->second;

		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

		_entries[key] += duration;

		_active.erase(it);
	}

    void Write(const std::string& path)
    {
        std::ofstream file(path);
        if (!file.is_open()) return;

        file << "{\n";

        bool firstCategory = true;

        std::unordered_map<std::string, std::vector<std::pair<std::string, int64_t>>> grouped;

        for (const auto& [key, value] : _entries)
        {
            const auto& [title, category] = key;
            grouped[category].push_back({ title, value });
        }

        for (const auto& [category, entries] : grouped)
        {
            if (!firstCategory) file << ",\n";
            firstCategory = false;

            file << "  \"" << category << "\": {\n";

            bool firstEntry = true;
            for (const auto& [title, value] : entries)
            {
                if (!firstEntry) file << ",\n";
                firstEntry = false;

                file << "    \"" << title << "\": " << value;
            }

            file << "\n  }";
        }

        file << "\n}\n";
    }
};