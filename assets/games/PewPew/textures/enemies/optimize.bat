@echo off
"c:\program files\php\php.exe" optimize.bat
pause
exit
<?
$files = scandir(".");
foreach($files as $file)
{
	if(substr($file, -5) == ".json" && strpos($file, ".opt.") === FALSE)
		optimize($file);
}


function optimize($filename)
{
	$data = json_decode(implode(array_map(function($line) 
	{
		if(strpos($line, "//") === FALSE)
			return $line;
		return substr($line, 0, strpos($line, "//"));
	}, file($filename))), true);

	$im = imagecreatefrompng($data["texture"]);
	$frames = array();

	$cmd = "sspack /pow2 /image:bla.png /map:bla.txt ";
	for($i = 0; $i < count($data["frames"]); $i++)
	{
		$tmp = imagecreatetruecolor($data["frames"][$i]["size"][0], $data["frames"][$i]["size"][1]);
		imagealphablending($tmp, false);
		imagecopy($tmp, $im, 0,0, $data["frames"][$i]["pos"][0], $data["frames"][$i]["pos"][1], imagesx($tmp), imagesy($tmp));


		$left = 0;
		$right = 0;
		$top = 0;
		$bottom = 0;
		for($x = 0; $x < imagesx($tmp); $x++)
		{
			$ok = true;
			for($y = 0; $y < imagesy($tmp); $y++)
			{
				$a = imagecolorsforindex($tmp, imagecolorat($tmp, $x,$y))["alpha"];
				if($a != 127)
				{
					$ok = false;
					break;
				}
			}
			if($ok)
				$left = $x+1;
			else
				break;
		}

		for($x = 0; $x < imagesx($tmp); $x++)
		{
			$ok = true;
			for($y = 0; $y < imagesy($tmp); $y++)
			{
				$a = imagecolorsforindex($tmp, imagecolorat($tmp, imagesx($tmp)-1-$x,$y))["alpha"];
				if($a != 127)
				{
					$ok = false;
					break;
				}
			}
			if($ok)
				$right = $x+1;
			else
				break;
		}
	
		for($y = 0; $y < imagesy($tmp); $y++)
		{
			$ok = true;
			for($x = 0; $x < imagesx($tmp); $x++)
			{
				$a = imagecolorsforindex($tmp, imagecolorat($tmp, $x,$y))["alpha"];
				if($a != 127)
				{
					$ok = false;
					break;
				}
			}
			if($ok)
				$top = $y+1;
			else
				break;
		}

		for($y = 0; $y < imagesy($tmp); $y++)
		{
			$ok = true;
			for($x = 0; $x < imagesx($tmp); $x++)
			{
				$a = imagecolorsforindex($tmp, imagecolorat($tmp, $x,imagesy($tmp)-1-$y))["alpha"];
				if($a != 127)
				{
					$ok = false;
					break;
				}
			}
			if($ok)
				$bottom = $y+1;
			else
				break;
		}



		$tmp2 = imagecreatetruecolor(imagesx($tmp) - $left - $right, imagesy($tmp) - $top - $bottom);
		imagealphablending($tmp2, false);
		imagecopy($tmp2, $tmp, 0, 0, $left, $top, imagesx($tmp2), imagesy($tmp2));


		imagesavealpha($tmp2, true);
		imagepng($tmp2, "bla" . $i . ".png");
		$cmd .= "bla" . $i . ".png ";
	}

	exec($cmd);


	for($i = 0; $i < count($data["frames"]); $i++)
	{
		unlink("bla" . $i . ".png");
	}


	$f = file("bla.txt");
	for($i = 0; $i < count($f); $i++)
	{
		$index = (int)substr($f[$i],3);
		$line = explode(" ", $f[$i]);

		$data["frames"][$index]["pos"][0] = (int)$line[2];
		$data["frames"][$index]["pos"][1] = (int)$line[3];

		$data["frames"][$index]["size"][0] = (int)$line[4];
		$data["frames"][$index]["size"][1] = (int)$line[5];
	}
	rename("bla.png", $data["texture"] . ".opt.png");
	$data["texture"] .= ".opt.png";
	
	file_put_contents($filename . ".opt.json", json_encode($data));
	unlink("bla.txt");
}


?>