#ifdef SETTING

SETTING(bool, BoolValue, AutoFix, Options, true, 0);

SETTING(s32, LongValue, LogLevel, Log, 2,
	L"# LogLevel\n"
	L"#\n"
	L"#  0 = trace\n"
	L"#  1 = debug\n"
	L"#  2 = info (default)\n"
	L"#  3 = warning\n"
	L"#  4 = error\n"
	L"#  5 = critical"
);

SETTING(bool, BoolValue, LogFlush, Log, false,
	L"# LogFlush\n"
	L"#\n"
	L"#  true = flush log file every line (very slow!)\n"
	L"#  false = flush log file only when error level message is logged (default)"
);

#endif
