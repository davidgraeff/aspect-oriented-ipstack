$VAR=	{
	'vname' => $APPNAME,
	'name' => "app_$APPNAME",
	'dir'  => "app/$APPNAME",
	'comp' => [
	{
		'vname' => 'Common Files',
		'files' => [
			"$APPNAME.config",
			"CMakeLists.txt",
		],
	},
  {
		'vname' => 'config.xml',
		'generate'=>'generate_config_xml();',
		'file' => 'config.xml',
	},
	{
		'vname' => 'appcode',
		'files' => [
			'appcode.h',
			'appcode.cpp',
		],
	},
	],
};
