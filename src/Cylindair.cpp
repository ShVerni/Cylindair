#include "Cylindair.h"

/// @brief Creates a new Cylindair object
/// @param Name The device name
Cylindair::Cylindair(String Name) : Sensor(Name) {}


/// @brief Starts the Cylindair
/// @return True on success
bool Cylindair::begin() {
	sensor_values.Parameters.resize(2);
	Description.parameterQuantity = 1;
	Description.type = "Air quality monitor";
	Description.parameters = {"AQI"};
	Description.units = {"level"};
	values.resize(Description.parameterQuantity);

	// Create settings directory if necessary
	if (!checkConfig(config_path)) {
		// Set defaults
		return setConfig(getConfig(), true);
	} else {
		// Load settings
		return setConfig(Storage::readFile(config_path), false);
	}
}

/// @brief Takes a measurement
/// @return True on success
bool Cylindair::takeMeasurement() {
	std::map<String, std::map<String, double>> params = sensor_values.getParameterValues();
	
	// Check if PM25 is enabled (not set to "None")
	bool pm25_enabled = sensor_values.Parameters.size() > 0 && !sensor_values.Parameters[0].first.isEmpty();
	
	// Extract PM25 value
	double pm25_value = 0;
	if (pm25_enabled && params.count(sensor_values.Parameters[0].first) > 0 && params[sensor_values.Parameters[0].first].count(sensor_values.Parameters[0].second) > 0) {
		pm25_value = params[sensor_values.Parameters[0].first][sensor_values.Parameters[0].second];
	}
	
	// Check if VOC is enabled (not set to "None")
	bool voc_enabled = sensor_values.Parameters.size() > 1 && !sensor_values.Parameters[1].first.isEmpty();
	
	// Extract tvoc_aqi value
	double tvoc_aqi = 0;
	if (voc_enabled && 	params.count(sensor_values.Parameters[1].first) > 0 && params[sensor_values.Parameters[1].first].count(sensor_values.Parameters[1].second) > 0) {
		tvoc_aqi = params[sensor_values.Parameters[1].first][sensor_values.Parameters[1].second];
	}

	// Calculate PM25 AQI index (only if PM25 is enabled)
	int pm25_aqi = 0;
	if (pm25_enabled) {
		pm25_aqi = 1;
		if (pm25_value > 12 && pm25_value <= 35) {
			pm25_aqi = 2;
		} else if (pm25_value > 35 && pm25_value <= 55) {
			pm25_aqi = 3;
		} else if (pm25_value > 55 && pm25_value <= 150) {
			pm25_aqi = 4;
		} else if (pm25_value > 150 && pm25_value <= 250) {
			pm25_aqi = 5;
		} else if (pm25_value > 250) {
			pm25_aqi = 6;
		}
	}
	
	// Use the higher of the two AQI values if both enabled, otherwise use the available one
	if (pm25_enabled && voc_enabled) {
		values[0] = tvoc_aqi > pm25_aqi ? tvoc_aqi : pm25_aqi;
	} else if (pm25_enabled) {
		values[0] = pm25_aqi;
	} else if (voc_enabled) {
		values[0] = tvoc_aqi;
	} else {
		values[0] = 0;
	}
	return true;
}

/// @brief Gets the current config
/// @return A JSON string of the config
String Cylindair::getConfig() {
	// Allocate the JSON document
	JsonDocument doc;

	// Add PM25 configuration - show "None" if not set
	if (sensor_values.Parameters.size() > 0 && !sensor_values.Parameters[0].first.isEmpty()) {
		doc["PM25"]["current"] = sensor_values.Parameters[0].first + ":" + sensor_values.Parameters[0].second;
	} else {
		doc["PM25"]["current"] = "None";
	}
	
	// Add VOC_AQI configuration - show "None" if not set
	if (sensor_values.Parameters.size() > 1 && !sensor_values.Parameters[1].first.isEmpty()) {
		doc["TVOC_AQI"]["current"] = sensor_values.Parameters[1].first + ":" + sensor_values.Parameters[1].second;
	} else {
		doc["TVOC_AQI"]["current"] = "None";
	}

	doc["PM25"]["options"][0] = "None";
	doc["TVOC_AQI"]["options"][0] = "None";
	// Get all available sensor parameters and build option lists
	std::map<String, std::vector<String>> sensors = sensor_values.listAllParameters();
	if (sensors.size() > 0) {
		int i = 1;
		for (std::map<String, std::vector<String>>::iterator sensor = sensors.begin(); sensor != sensors.end(); sensor++) {
			for (const auto& p : sensor->second) {
				doc["TVOC_AQI"]["options"][i] = sensor->first + ":" + p;
				doc["PM25"]["options"][i] = sensor->first + ":" + p;
				i++;
			}
		}
	}
	// Create string to hold output
	String output;
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool Cylindair::setConfig(String config, bool save) {
	// Allocate the JSON document
	JsonDocument doc;
	// Deserialize file contents
	DeserializationError error = deserializeJson(doc, config);
	// Test if parsing succeeds.
	if (error) {
		Logger.print(F("Deserialization failed: "));
		Logger.println(error.f_str());
		return false;
	}
	// Parse PM25 parameter
	String sensor_combined = doc["PM25"]["current"].as<String>();
	if (sensor_combined.isEmpty() || sensor_combined == "None") {
		// Set to empty pair to indicate "None"
		sensor_values.Parameters[0] = {"", ""};
	} else {
		int colon;
		if ((colon = sensor_combined.indexOf(':')) != -1) {
			std::pair<String, String> chosen {sensor_combined.substring(0, colon), sensor_combined.substring(colon + 1)};
			sensor_values.Parameters[0] = chosen;
		}
	}

	// Parse TVOC_AQI parameter
	sensor_combined = doc["TVOC_AQI"]["current"].as<String>();
	if (sensor_combined.isEmpty() || sensor_combined == "None") {
		// Set to empty pair to indicate "None"
		sensor_values.Parameters[1] = {"", ""};
	} else {
		int colon;
		if ((colon = sensor_combined.indexOf(':')) != -1) {
			std::pair<String, String> chosen {sensor_combined.substring(0, colon), sensor_combined.substring(colon + 1)};
			sensor_values.Parameters[1] = chosen;
		}
	}

	if (save) {
		return saveConfig(config_path, config);
	}
	return true;
}