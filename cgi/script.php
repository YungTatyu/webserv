<?php
	echo "<h3>Hello from PHP script!</h3>";
	echo "<p>Current time: " . date(DATE_COOKIE, time()) . "</p>";
	echo "<p>Query String = \"" . getenv('QUERY_STRING') . "\"</p>";
?>
