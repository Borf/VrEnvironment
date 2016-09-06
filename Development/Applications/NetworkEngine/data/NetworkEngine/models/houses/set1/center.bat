"c:\program files\php\php.exe" %0
pause
<?

$files = scandir(".");
foreach($files as $file)
{
	if(substr($file, 0, 5) != "house")
		continue;

	if(substr($file, -4,4) != ".obj")
		continue;

	if(substr($file, -8,8) == ".new.obj")
		continue;

	$f = file_get_contents($file);
	$lines = explode("\n", $f);

	$center = array(0,0,0);
	$count = 0;	
	foreach($lines as $line)
	{
		if(substr(trim($line),0,2) == "v ")
		{
			$p = explode(" ", $line);
			$count++;
			for($i = 0; $i < 3; $i++)
				$center[$i] += $p[$i+1];
		}
	}
	
	for($i = 0; $i < 3; $i++)
		$center[$i] /= $count;

	$out = "";
	foreach($lines as $line)
	{
		if(substr(trim($line),0,2) == "v ")
		{
			$p = explode(" ", $line);
			$out .= "v " . ($p[1]-$center[0]) . " " . ($p[2]) . " " . ($p[3]-$center[2]) . "\n";
		}
		else
			$out .= trim($line) . "\n";

	}
	echo $file . "\n";
	print_r($center);

	//file_put_contents($file . ".new.obj", $out);
	file_put_contents($file, $out);

}


?>