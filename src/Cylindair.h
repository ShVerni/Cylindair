/*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2026 Sam Groveman
* 
* External libraries needed:
* ParameterGetter: https://github.com/FabricaIO/util-ParameterGetter/
* 
* Contributors: Sam Groveman
*/

#pragma once
#include <Sensor.h>
#include <ParameterGetter.h>

/// @brief Sensor for aggregating AQI from PM2.5 and TVOC sensor
class Cylindair : public Sensor {
	public:
		Cylindair(String Name);
		bool begin();
		bool takeMeasurement();
		String getConfig();
		bool setConfig(String config, bool save);
		
	protected:
		/// @brief Cylindair sensor configuration

		/// @brief Provides access to the sensor values
		ParameterGetter sensor_values;

		/// @brief Full path to config file
		const String config_path = "/settings/sen/cylindair.json";
};