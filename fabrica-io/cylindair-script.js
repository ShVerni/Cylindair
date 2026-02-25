/*
 * This file is licensed under the GPLv3 License Copyright (c) 2026 Sam Groveman
 * Contributors: Sam Groveman
 */

// Run code when page DOM is loaded
document.addEventListener("DOMContentLoaded", () => {
	// Attach button handlers

	/// Delete saved data
	document.getElementById("delete").onclick = function() {
		if (confirm("Delete data?")) {
			POSTRequest("/delete", "Data deleted!", { path: "/data/Data.csv" });
		}
	};
});