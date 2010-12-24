<?php
require_once ('Struct.inc.php');

$_ENV['BBSHOME'] = '/home/bbs';

class BBSIO
{

	function checkpwd_bbs($userid, $passwd)
	{
		$fn = '/tmp/uc_' . uniqid();
		system('/home/bbs/bin/checkpwd ' . escapeshellarg($userid) . ' ' . escapeshellarg($passwd) . ' > ' . $fn);
		$r = file_get_contents($fn);
		unlink($fn);
		return substr($r, 0, 1) === '0';
	}

	function user_exists($userid, $passwd = '')
	{
		$fn = '/tmp/uc_' . uniqid();
		system('/home/bbs/bin/checkpwd ' . escapeshellarg($userid) . ' ' . escapeshellarg($passwd) . ' > ' . $fn);
		$r = file_get_contents($fn);
		unlink($fn);
		return substr($r, 0, 1) !== '2';
	}

	function checkpwd_bbs2($userid, $passwd)
	{
		$fn = '/tmp/uc_' . uniqid();
		system('/home/bbs/bin/checkpwd ' . escapeshellarg($userid) . ' ' . escapeshellarg($passwd) . ' > ' . $fn);
		$r = file_get_contents($fn);
		unlink($fn);
		return substr($r, 0, 1);
	}

	function homePath($userid)
	{
		$x = substr($userid, 0, 1);
		return $_ENV['BBSHOME'] . "/home/$x/$userid/";
	}

	function setAuth($userid)
	{
		system ("mkdir -p /tmp/bbsauth/ &> /dev/null");
		$auth = '/tmp/bbsauth/' . strtolower($userid) ;
		$uuid = uniqid();
		file_put_contents($auth, $uuid);
		return $uuid;
	}

	function checkAuth($userid, $uuid)
	{
		$auth = '/tmp/bbsauth/' . strtolower($userid) ;
		if (filemtime ($auth) < time() - 3600 * 2)
		{
			return false;
		}
		return $uuid && (file_get_contents($auth) == $uuid);
	}

	function removeAuth($userid)
	{
		$auth = '/tmp/bbsauth/' . strtolower($userid);
		unlink($auth);
	}

	function getBBSInfo($userid)
	{
		$pwd = $_ENV['BBSHOME'] . '/.PASSWDS';
		$s_userec = Struct::LoadStruct('userec');
		$l = $s_userec->SizeOf();
		$fp = fopen($pwd, 'rb');
		if (!$fp)
		{
			return null;
		}
		while ($buf = fread($fp, $l))
		{
			$o = $s_userec->Unpack($buf);
			if (strtolower($o['userid']) == strtolower($userid))
			{
				fclose ($fp);
				return $o;
			}
		}
		fclose($fp);
		return null;
	}

};

