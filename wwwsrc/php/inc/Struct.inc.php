<?php

class Struct
{
	var $_sinfo, $_sizebuf;
	static $ITEMLIST = 
		array(
			'int' => array('c' => 'i', 's' => 4),
			'long' => array('c' => 'i', 's' => 4),
			'float' => array('c' => 'f', 's' => 4),
			'double' => array('c' => 'd', 's' => 8),
			'char' => array('c' => 'a', 's' => 1),
			'byte' => array('c' => 'c', 's' => 1),
			'short' => array('c' => 's', 's' => 2)
		);
	static $STRUCTOBJS = array();

	function Struct($init = null, $name = '')
	{
		$_sinfo = array();
		$_sizebuf = -1;
		if ($name)
		{
			self::$STRUCTOBJS[$name] = &$this;
		}
		if (is_array($init))
		{
			foreach ($init as $k => $v)
			{
				$n = $t = $l = '';
				if (isset($v['n']))
				{
					$n = $v['n'];
					$t = $v['t'];
					$l = $v['l'];
				}
				else 
				{
					$n = $v[0];
					$t = $v[1];
					$l = $v[2];
				}
				if (!$l)
				{
					$l = 1;
				}
				if (is_numeric($t))
				{
					$l = $t;
					$t = 'char';
				}
				$this->AddItem($n, $t, $l);
			}
		}
	}

	function ItemValid($type, $count)
	{
		if (!$count || $count <= 0)
		{
			return false;
		}
		if (!is_int($count))
		{
			return false;
		}
		if (is_string($type))
		{
			if (isset(self::$ITEMLIST[$type]))
			{
				return $type;
			}
			if ($t = &self::LoadStruct($type))
			{
				return $t;
			}
			return false;
		}
		else
		{
			if ($type instanceof Struct)
			{
				return $type;
			}
		}
		return false;
	}
	
	function AddItem($name, $type, $count = 1)
	{	
		if (!$count)
		{
			$count = 1;
		}
		if ($t = $this->ItemValid($type, $count))
		{
			$this->_sizebuf = -1;
			$this->_sinfo[] = array('n' => $name, 't' => $t, 'l' => $count);
			return true;
		}
		return false;
	}

	static function LoadStruct($type)
	{
		if (isset(self::$STRUCTOBJS[$type]))
		{
			return self::$STRUCTOBJS[$type];
		}
		else
		{
			$fn = "structs/$type.struct.php";
			$o = null;
			@include($fn);
			if ($o)
			{
				return (new Struct($o));
			}
		}
		return null;
	}

	function SizeOfItem($v)
	{
		return (($v['t'] instanceof Struct) ? $v['t']->SizeOf() : self::$ITEMLIST[$v['t']]['s']) * $v['l'];
	}

	function SizeOf()
	{
		if ($this->_sizebuf >= 0)
		{
			return $this->_sizebuf;
		}
		$s = 0;
		foreach ($this->_sinfo as $k => $v)
		{
			$s += $this->SizeOfItem($v);
		}
		return $this->_sizebuf = $s;
	}

	function Pack($arr)
	{
		$b = '';
		foreach ($this->_sinfo as $v)
		{
			if ($v['t'] instanceof Struct)
			{
				if ($v['l'] > 1)
				{
					for ($i = 0; $i < $v['l']; $i++)
					{
						$b .= $v['t']->Pack($arr[$v['n']][$i]);
					}
				}
				else
				{
					$b .= $v['t']->Pack($arr[$v['n']]);
				}
			}
			else
			{
				$p = self::$ITEMLIST[$v['t']]['c'];
				$p .= $v['l'];
				if (is_array($arr[$v['n']]))
				{
					$x = $arr[$v['n']];
					array_unshift($x, $p);
					$d = @call_user_func_array('pack', $x);
					unset($x);
				}
				else
				{
					$d = @pack($p, $arr[$v['n']]);
				}
				if (strlen($d) < $this->SizeOfItem($v))
				{
					//warning
					$d = pack('a' . $this->SizeOfItem($v), '');
				}
				$b .= $d;
			}
		}
		return $b;
	}

	function Unpack($bin, $start = 0)
	{
		$pos = $start;
		$o = array();
		foreach ($this->_sinfo as $v)
		{
			if ($v['t'] instanceof Struct)
			{
				if ($v['l'] > 1)
				{
					$o[$v['n']] = array();
					for ($i = 0; $i < $v['l']; $i++)
					{
						$o[$v['n']][$i] = $v['t']->Unpack($bin, $pos);
						$pos += $v['t']->SizeOf();
					}
				}
				else
				{
					$o[$v['n']] = $v['t']->Unpack($bin, $pos);
					$pos += $v['t']->SizeOf();
				}
			}
			else
			{
				$p = self::$ITEMLIST[$v['t']]['c'];
				$p .= $v['l'];
				$s = $this->SizeOfItem($v);
				$d = @unpack($p, substr($bin, $pos, $s));
				if (count($d) == 1)
				{
					$d = $d[1];//1!
				}
				else
				{
					$d = array_merge($d, array());
				}
				$o[$v['n']] = $d;
				$pos += $s;
			}
			//echo $pos;
			//var_dump($o[$v['n']]);
		}
		return $o;
	}

}

?>
