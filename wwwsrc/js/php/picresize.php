<?php
//sprintf(buf,"/phpbbs/picresize.php?tmpfile=%d&filename=%s&board=%s",getpid(),filename,board);
//int pid=atoi(getparm("tmpfile"));
//sprintf(tmpfile, "tmp/%d.upload", pid);
	include('phpthumb/phpthumb.bmp.php');
	function makethumb($srcFile,$dstW,$dstH)
	{
		$force=false;
		$data = GetImageSize($srcFile,&$info);
		switch ($data[2]) 
		{
			case 1: //图片类型，1是GIF图
				return false;
				$im = @ImageCreateFromGIF($srcFile);
				break;
			case 2: //图片类型，2是JPG图
				$im = @imagecreatefromjpeg($srcFile);
				break;
			case 3: //图片类型，3是PNG图
				return false;
				$im = @ImageCreateFromPNG($srcFile);
				break;
			case 6:
				$pb=new phpthumb_bmp();
				$im=$pb->phpthumb_bmpfile2gd($srcFile);
				break;
			default:
				return false;
		}
		$srcW=ImageSX($im);
		$srcH=ImageSY($im);
		$w=$dstW;
		$h=$dstH;
		if($srcW==0||$srcH==0)return false;
		$r=$srcW/$srcH;
		if($w/$h>$r)$w=$h*$r;else $h=$w/$r;
		if($srcW<=$w&&$data[2]!=6)
		{
			$w=$srcW;
			$h=$srcH;
		}
		$ni=ImageCreateTrueColor($w,$h);
		if($data[2]==2)
		{
			ImageCopyResized($ni,$im,0,0,0,0,$w,$h,$srcW,$srcH);
			ImageJPEG($ni,$srcFile);
		}
		else
			ImagePNG($im,$srcFile);
		return $data[2];
	}

	$pid='/home/bbs/tmp/' . $_REQUEST['tmpfile'] . '.upload';
	$srcFile=$pid;
	//取得文件扩展名：
	$fn=$_REQUEST['filename'];
	if($ext=makethumb($srcFile,640,480))
	{
		if($ext==2)
			$ext='jpg';
		else
			$ext='png';
		if(strchr($fn,"."))
			$fn=substr($fn,0,strlen($fn)-strlen(strrchr($fn,"."))+1).$ext;
		else
			$fn=$fn.$ext;
	}
	$host  = $_SERVER['HTTP_HOST'];
	$extra = 'cgi-bin/bbs/bbsupload?tmpfile='.$_REQUEST['tmpfile'].'&filename='.urlencode($fn).'&board='.$_REQUEST['board'];
	header("Location: http://$host/$extra");
	exit;
?>
