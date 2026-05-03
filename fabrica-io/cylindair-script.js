/*
 * This file is licensed under the GPLv3 License Copyright (c) 2026 Sam Groveman
 * Contributors: Sam Groveman
 */

// Run code when page DOM is loaded
document.addEventListener("DOMContentLoaded", () => {
	// Attach button handlers

	/// Delete saved data
	document.getElementById("delete").onclick = async function() {
		if (confirm("Delete data?")) {
			try {
				await POSTRequest("/delete", "Data deleted!", { path: "/data/Data.csv" });
			} catch (e) {
				document.getElementById("message").html = "Could not delete file: " + e;
				console.error(e);
			}
		}
	};
});