use std::process::exit;
fn check_0(cur: i64) -> i64 {
  if 0 == cur { exit(1) };
  let mut cur = cur;
  cur = cur - 33; // 66 - 33 = 33
  cur = cur + 9; // 33 + 9 = 42
  cur = cur + 25; // 42 + 25 = 67
  cur = cur + 8; // 67 + 8 = 75
  cur = cur - 22; // 75 - 22 = 53
  cur = cur + 26; // 53 + 26 = 79
  cur = cur + 29; // 79 + 29 = 108
  cur = cur + 30; // 108 + 30 = 138
  cur = cur - 31; // 138 - 31 = 107
  cur = cur + 34; // 107 + 34 = 141
  cur = cur - 17; // 141 - 17 = 124
  cur = cur - 3; // 124 - 3 = 121
  cur = cur + 21; // 121 + 21 = 142
  cur = cur - 8; // 142 - 8 = 134
  cur = cur + 16; // 134 + 16 = 150
  cur = cur - 3; // 150 - 3 = 147
  cur = cur + 22; // 147 + 22 = 169
  cur = cur - 26; // 169 - 26 = 143
  cur = cur + 19; // 143 + 19 = 162
  cur = cur + 13; // 162 + 13 = 175
  cur = cur + 22; // 175 + 22 = 197
  cur = cur + 20; // 197 + 20 = 217
  cur = cur + 5; // 217 + 5 = 222
  cur = cur - 8; // 222 - 8 = 214
  cur = cur - 14; // 214 - 14 = 200
  cur = cur + 23; // 200 + 23 = 223
  cur = cur + 31; // 223 + 31 = 254
  cur = cur + 9; // 254 + 9 = 263
  cur = cur - 10; // 263 - 10 = 253
  cur = cur - 11; // 253 - 11 = 242
  cur = cur + 31; // 242 + 31 = 273
  cur = cur + 23; // 273 + 23 = 296
  if 296 != cur { exit(1); }
  return cur;
}

fn check_1(cur: i64) -> i64 {
  if 0 == cur { exit(2) };
  let mut cur = cur;
  cur = cur - 15; // 111 - 15 = 96
  cur = cur + 23; // 96 + 23 = 119
  cur = cur + 28; // 119 + 28 = 147
  cur = cur - 22; // 147 - 22 = 125
  cur = cur - 12; // 125 - 12 = 113
  cur = cur - 26; // 113 - 26 = 87
  cur = cur + 4; // 87 + 4 = 91
  cur = cur - 31; // 91 - 31 = 60
  cur = cur + 29; // 60 + 29 = 89
  cur = cur + 4; // 89 + 4 = 93
  cur = cur + 33; // 93 + 33 = 126
  cur = cur - 23; // 126 - 23 = 103
  cur = cur - 15; // 103 - 15 = 88
  cur = cur + 30; // 88 + 30 = 118
  cur = cur + 24; // 118 + 24 = 142
  cur = cur + 14; // 142 + 14 = 156
  cur = cur - 15; // 156 - 15 = 141
  cur = cur + 25; // 141 + 25 = 166
  cur = cur - 25; // 166 - 25 = 141
  cur = cur - 13; // 141 - 13 = 128
  cur = cur - 17; // 128 - 17 = 111
  cur = cur - 34; // 111 - 34 = 77
  cur = cur + 23; // 77 + 23 = 100
  cur = cur - 25; // 100 - 25 = 75
  cur = cur - 8; // 75 - 8 = 67
  cur = cur - 13; // 67 - 13 = 54
  cur = cur + 18; // 54 + 18 = 72
  cur = cur - 25; // 72 - 25 = 47
  cur = cur + 12; // 47 + 12 = 59
  cur = cur + 10; // 59 + 10 = 69
  cur = cur - 6; // 69 - 6 = 63
  cur = cur + 13; // 63 + 13 = 76
  if 76 != cur { exit(2); }
  return cur;
}

fn check_2(cur: i64) -> i64 {
  if 0 == cur { exit(3) };
  let mut cur = cur;
  cur = cur - 18; // 105 - 18 = 87
  cur = cur - 8; // 87 - 8 = 79
  cur = cur - 13; // 79 - 13 = 66
  cur = cur - 27; // 66 - 27 = 39
  cur = cur - 30; // 39 - 30 = 9
  cur = cur - 22; // 9 - 22 = -13
  cur = cur + 13; // -13 + 13 = 0
  cur = cur - 33; // 0 - 33 = -33
  cur = cur - 8; // -33 - 8 = -41
  cur = cur + 21; // -41 + 21 = -20
  cur = cur - 30; // -20 - 30 = -50
  cur = cur + 11; // -50 + 11 = -39
  cur = cur - 23; // -39 - 23 = -62
  cur = cur + 19; // -62 + 19 = -43
  cur = cur - 28; // -43 - 28 = -71
  cur = cur - 23; // -71 - 23 = -94
  cur = cur - 30; // -94 - 30 = -124
  cur = cur - 23; // -124 - 23 = -147
  cur = cur - 3; // -147 - 3 = -150
  cur = cur - 13; // -150 - 13 = -163
  cur = cur + 8; // -163 + 8 = -155
  cur = cur - 18; // -155 - 18 = -173
  cur = cur + 17; // -173 + 17 = -156
  cur = cur - 7; // -156 - 7 = -163
  cur = cur - 34; // -163 - 34 = -197
  cur = cur + 3; // -197 + 3 = -194
  cur = cur + 5; // -194 + 5 = -189
  cur = cur + 32; // -189 + 32 = -157
  cur = cur + 5; // -157 + 5 = -152
  cur = cur - 3; // -152 - 3 = -155
  cur = cur - 32; // -155 - 32 = -187
  cur = cur + 4; // -187 + 4 = -183
  if -183 != cur { exit(3); }
  return cur;
}

fn check_3(cur: i64) -> i64 {
  if 0 == cur { exit(4) };
  let mut cur = cur;
  cur = cur - 12; // 110 - 12 = 98
  cur = cur - 4; // 98 - 4 = 94
  cur = cur - 17; // 94 - 17 = 77
  cur = cur + 32; // 77 + 32 = 109
  cur = cur - 5; // 109 - 5 = 104
  cur = cur - 32; // 104 - 32 = 72
  cur = cur + 8; // 72 + 8 = 80
  cur = cur + 9; // 80 + 9 = 89
  cur = cur + 28; // 89 + 28 = 117
  cur = cur - 20; // 117 - 20 = 97
  cur = cur + 27; // 97 + 27 = 124
  cur = cur - 3; // 124 - 3 = 121
  cur = cur + 21; // 121 + 21 = 142
  cur = cur + 21; // 142 + 21 = 163
  cur = cur - 16; // 163 - 16 = 147
  cur = cur + 12; // 147 + 12 = 159
  cur = cur - 22; // 159 - 22 = 137
  cur = cur - 5; // 137 - 5 = 132
  cur = cur + 19; // 132 + 19 = 151
  cur = cur + 21; // 151 + 21 = 172
  cur = cur - 11; // 172 - 11 = 161
  cur = cur + 28; // 161 + 28 = 189
  cur = cur + 11; // 189 + 11 = 200
  cur = cur + 33; // 200 + 33 = 233
  cur = cur + 19; // 233 + 19 = 252
  cur = cur + 7; // 252 + 7 = 259
  cur = cur + 9; // 259 + 9 = 268
  cur = cur + 21; // 268 + 21 = 289
  cur = cur - 30; // 289 - 30 = 259
  cur = cur + 15; // 259 + 15 = 274
  cur = cur + 30; // 274 + 30 = 304
  cur = cur - 31; // 304 - 31 = 273
  if 273 != cur { exit(4); }
  return cur;
}

fn check_4(cur: i64) -> i64 {
  if 0 == cur { exit(5) };
  let mut cur = cur;
  cur = cur + 15; // 103 + 15 = 118
  cur = cur + 25; // 118 + 25 = 143
  cur = cur + 26; // 143 + 26 = 169
  cur = cur + 17; // 169 + 17 = 186
  cur = cur - 33; // 186 - 33 = 153
  cur = cur - 9; // 153 - 9 = 144
  cur = cur - 33; // 144 - 33 = 111
  cur = cur - 22; // 111 - 22 = 89
  cur = cur + 27; // 89 + 27 = 116
  cur = cur - 22; // 116 - 22 = 94
  cur = cur + 9; // 94 + 9 = 103
  cur = cur - 18; // 103 - 18 = 85
  cur = cur + 25; // 85 + 25 = 110
  cur = cur + 23; // 110 + 23 = 133
  cur = cur - 32; // 133 - 32 = 101
  cur = cur + 23; // 101 + 23 = 124
  cur = cur - 19; // 124 - 19 = 105
  cur = cur + 4; // 105 + 4 = 109
  cur = cur + 33; // 109 + 33 = 142
  cur = cur + 26; // 142 + 26 = 168
  cur = cur - 29; // 168 - 29 = 139
  cur = cur - 13; // 139 - 13 = 126
  cur = cur - 15; // 126 - 15 = 111
  cur = cur + 30; // 111 + 30 = 141
  cur = cur + 15; // 141 + 15 = 156
  cur = cur - 4; // 156 - 4 = 152
  cur = cur + 14; // 152 + 14 = 166
  cur = cur - 31; // 166 - 31 = 135
  cur = cur + 20; // 135 + 20 = 155
  cur = cur + 19; // 155 + 19 = 174
  cur = cur + 11; // 174 + 11 = 185
  cur = cur + 7; // 185 + 7 = 192
  if 192 != cur { exit(5); }
  return cur;
}

fn check_5(cur: i64) -> i64 {
  if 0 == cur { exit(6) };
  let mut cur = cur;
  cur = cur - 6; // 32 - 6 = 26
  cur = cur + 28; // 26 + 28 = 54
  cur = cur - 29; // 54 - 29 = 25
  cur = cur - 28; // 25 - 28 = -3
  cur = cur - 18; // -3 - 18 = -21
  cur = cur + 7; // -21 + 7 = -14
  cur = cur - 25; // -14 - 25 = -39
  cur = cur - 16; // -39 - 16 = -55
  cur = cur + 30; // -55 + 30 = -25
  cur = cur - 8; // -25 - 8 = -33
  cur = cur + 16; // -33 + 16 = -17
  cur = cur + 32; // -17 + 32 = 15
  cur = cur - 24; // 15 - 24 = -9
  cur = cur + 24; // -9 + 24 = 15
  cur = cur - 23; // 15 - 23 = -8
  cur = cur + 25; // -8 + 25 = 17
  cur = cur + 18; // 17 + 18 = 35
  cur = cur - 33; // 35 - 33 = 2
  cur = cur + 31; // 2 + 31 = 33
  cur = cur - 17; // 33 - 17 = 16
  cur = cur + 23; // 16 + 23 = 39
  cur = cur - 32; // 39 - 32 = 7
  cur = cur + 29; // 7 + 29 = 36
  cur = cur + 13; // 36 + 13 = 49
  cur = cur + 22; // 49 + 22 = 71
  cur = cur - 6; // 71 - 6 = 65
  cur = cur + 32; // 65 + 32 = 97
  cur = cur - 3; // 97 - 3 = 94
  cur = cur + 27; // 94 + 27 = 121
  cur = cur + 30; // 121 + 30 = 151
  cur = cur + 16; // 151 + 16 = 167
  cur = cur + 27; // 167 + 27 = 194
  if 194 != cur { exit(6); }
  return cur;
}

fn check_6(cur: i64) -> i64 {
  if 0 == cur { exit(7) };
  let mut cur = cur;
  cur = cur - 8; // 40 - 8 = 32
  cur = cur + 6; // 32 + 6 = 38
  cur = cur - 34; // 38 - 34 = 4
  cur = cur + 26; // 4 + 26 = 30
  cur = cur + 32; // 30 + 32 = 62
  cur = cur + 31; // 62 + 31 = 93
  cur = cur - 18; // 93 - 18 = 75
  cur = cur - 32; // 75 - 32 = 43
  cur = cur - 22; // 43 - 22 = 21
  cur = cur + 32; // 21 + 32 = 53
  cur = cur + 12; // 53 + 12 = 65
  cur = cur + 26; // 65 + 26 = 91
  cur = cur + 4; // 91 + 4 = 95
  cur = cur + 20; // 95 + 20 = 115
  cur = cur + 24; // 115 + 24 = 139
  cur = cur - 13; // 139 - 13 = 126
  cur = cur - 33; // 126 - 33 = 93
  cur = cur + 14; // 93 + 14 = 107
  cur = cur - 20; // 107 - 20 = 87
  cur = cur + 23; // 87 + 23 = 110
  cur = cur - 18; // 110 - 18 = 92
  cur = cur + 13; // 92 + 13 = 105
  cur = cur - 4; // 105 - 4 = 101
  cur = cur - 17; // 101 - 17 = 84
  cur = cur + 5; // 84 + 5 = 89
  cur = cur - 11; // 89 - 11 = 78
  cur = cur - 28; // 78 - 28 = 50
  cur = cur - 3; // 50 - 3 = 47
  cur = cur + 8; // 47 + 8 = 55
  cur = cur - 25; // 55 - 25 = 30
  cur = cur + 9; // 30 + 9 = 39
  cur = cur - 18; // 39 - 18 = 21
  if 21 != cur { exit(7); }
  return cur;
}

fn check_7(cur: i64) -> i64 {
  if 0 == cur { exit(8) };
  let mut cur = cur;
  cur = cur - 27; // 104 - 27 = 77
  cur = cur + 20; // 77 + 20 = 97
  cur = cur - 17; // 97 - 17 = 80
  cur = cur + 3; // 80 + 3 = 83
  cur = cur + 34; // 83 + 34 = 117
  cur = cur - 13; // 117 - 13 = 104
  cur = cur + 24; // 104 + 24 = 128
  cur = cur - 8; // 128 - 8 = 120
  cur = cur - 24; // 120 - 24 = 96
  cur = cur + 11; // 96 + 11 = 107
  cur = cur + 15; // 107 + 15 = 122
  cur = cur - 11; // 122 - 11 = 111
  cur = cur - 33; // 111 - 33 = 78
  cur = cur + 4; // 78 + 4 = 82
  cur = cur + 6; // 82 + 6 = 88
  cur = cur + 31; // 88 + 31 = 119
  cur = cur + 17; // 119 + 17 = 136
  cur = cur - 11; // 136 - 11 = 125
  cur = cur - 22; // 125 - 22 = 103
  cur = cur - 17; // 103 - 17 = 86
  cur = cur - 9; // 86 - 9 = 77
  cur = cur - 18; // 77 - 18 = 59
  cur = cur + 14; // 59 + 14 = 73
  cur = cur + 34; // 73 + 34 = 107
  cur = cur + 19; // 107 + 19 = 126
  cur = cur - 22; // 126 - 22 = 104
  cur = cur - 33; // 104 - 33 = 71
  cur = cur - 7; // 71 - 7 = 64
  cur = cur + 10; // 64 + 10 = 74
  cur = cur + 8; // 74 + 8 = 82
  cur = cur - 31; // 82 - 31 = 51
  cur = cur - 13; // 51 - 13 = 38
  if 38 != cur { exit(8); }
  return cur;
}

fn check_8(cur: i64) -> i64 {
  if 0 == cur { exit(9) };
  let mut cur = cur;
  cur = cur + 30; // 116 + 30 = 146
  cur = cur + 8; // 146 + 8 = 154
  cur = cur - 3; // 154 - 3 = 151
  cur = cur + 5; // 151 + 5 = 156
  cur = cur + 15; // 156 + 15 = 171
  cur = cur + 14; // 171 + 14 = 185
  cur = cur + 25; // 185 + 25 = 210
  cur = cur + 21; // 210 + 21 = 231
  cur = cur + 21; // 231 + 21 = 252
  cur = cur + 30; // 252 + 30 = 282
  cur = cur + 12; // 282 + 12 = 294
  cur = cur + 32; // 294 + 32 = 326
  cur = cur + 32; // 326 + 32 = 358
  cur = cur - 21; // 358 - 21 = 337
  cur = cur - 28; // 337 - 28 = 309
  cur = cur + 3; // 309 + 3 = 312
  cur = cur + 9; // 312 + 9 = 321
  cur = cur + 6; // 321 + 6 = 327
  cur = cur - 12; // 327 - 12 = 315
  cur = cur + 18; // 315 + 18 = 333
  cur = cur - 30; // 333 - 30 = 303
  cur = cur + 18; // 303 + 18 = 321
  cur = cur - 20; // 321 - 20 = 301
  cur = cur - 16; // 301 - 16 = 285
  cur = cur - 22; // 285 - 22 = 263
  cur = cur - 24; // 263 - 24 = 239
  cur = cur - 21; // 239 - 21 = 218
  cur = cur + 25; // 218 + 25 = 243
  cur = cur - 24; // 243 - 24 = 219
  cur = cur + 16; // 219 + 16 = 235
  cur = cur + 16; // 235 + 16 = 251
  cur = cur + 32; // 251 + 32 = 283
  if 283 != cur { exit(9); }
  return cur;
}

fn check_9(cur: i64) -> i64 {
  if 0 == cur { exit(10) };
  let mut cur = cur;
  cur = cur - 15; // 116 - 15 = 101
  cur = cur - 29; // 101 - 29 = 72
  cur = cur - 33; // 72 - 33 = 39
  cur = cur + 3; // 39 + 3 = 42
  cur = cur - 6; // 42 - 6 = 36
  cur = cur + 17; // 36 + 17 = 53
  cur = cur + 25; // 53 + 25 = 78
  cur = cur + 11; // 78 + 11 = 89
  cur = cur - 13; // 89 - 13 = 76
  cur = cur - 34; // 76 - 34 = 42
  cur = cur - 17; // 42 - 17 = 25
  cur = cur + 29; // 25 + 29 = 54
  cur = cur + 18; // 54 + 18 = 72
  cur = cur + 27; // 72 + 27 = 99
  cur = cur + 4; // 99 + 4 = 103
  cur = cur + 34; // 103 + 34 = 137
  cur = cur - 4; // 137 - 4 = 133
  cur = cur + 11; // 133 + 11 = 144
  cur = cur - 31; // 144 - 31 = 113
  cur = cur - 33; // 113 - 33 = 80
  cur = cur + 25; // 80 + 25 = 105
  cur = cur - 4; // 105 - 4 = 101
  cur = cur - 31; // 101 - 31 = 70
  cur = cur - 24; // 70 - 24 = 46
  cur = cur + 23; // 46 + 23 = 69
  cur = cur + 29; // 69 + 29 = 98
  cur = cur + 8; // 98 + 8 = 106
  cur = cur - 34; // 106 - 34 = 72
  cur = cur + 32; // 72 + 32 = 104
  cur = cur + 11; // 104 + 11 = 115
  cur = cur - 9; // 115 - 9 = 106
  cur = cur - 25; // 106 - 25 = 81
  if 81 != cur { exit(10); }
  return cur;
}

fn check_10(cur: i64) -> i64 {
  if 0 == cur { exit(11) };
  let mut cur = cur;
  cur = cur - 9; // 112 - 9 = 103
  cur = cur + 24; // 103 + 24 = 127
  cur = cur + 12; // 127 + 12 = 139
  cur = cur + 9; // 139 + 9 = 148
  cur = cur + 12; // 148 + 12 = 160
  cur = cur - 22; // 160 - 22 = 138
  cur = cur - 24; // 138 - 24 = 114
  cur = cur + 22; // 114 + 22 = 136
  cur = cur + 20; // 136 + 20 = 156
  cur = cur + 29; // 156 + 29 = 185
  cur = cur - 10; // 185 - 10 = 175
  cur = cur + 21; // 175 + 21 = 196
  cur = cur + 5; // 196 + 5 = 201
  cur = cur - 32; // 201 - 32 = 169
  cur = cur + 22; // 169 + 22 = 191
  cur = cur + 25; // 191 + 25 = 216
  cur = cur + 25; // 216 + 25 = 241
  cur = cur - 27; // 241 - 27 = 214
  cur = cur - 13; // 214 - 13 = 201
  cur = cur - 12; // 201 - 12 = 189
  cur = cur - 33; // 189 - 33 = 156
  cur = cur - 32; // 156 - 32 = 124
  cur = cur + 33; // 124 + 33 = 157
  cur = cur + 22; // 157 + 22 = 179
  cur = cur + 34; // 179 + 34 = 213
  cur = cur - 9; // 213 - 9 = 204
  cur = cur - 26; // 204 - 26 = 178
  cur = cur + 5; // 178 + 5 = 183
  cur = cur - 12; // 183 - 12 = 171
  cur = cur + 28; // 171 + 28 = 199
  cur = cur - 11; // 199 - 11 = 188
  cur = cur + 31; // 188 + 31 = 219
  if 219 != cur { exit(11); }
  return cur;
}

fn check_11(cur: i64) -> i64 {
  if 0 == cur { exit(12) };
  let mut cur = cur;
  cur = cur + 16; // 58 + 16 = 74
  cur = cur + 19; // 74 + 19 = 93
  cur = cur - 24; // 93 - 24 = 69
  cur = cur + 24; // 69 + 24 = 93
  cur = cur - 12; // 93 - 12 = 81
  cur = cur - 27; // 81 - 27 = 54
  cur = cur - 10; // 54 - 10 = 44
  cur = cur + 3; // 44 + 3 = 47
  cur = cur - 17; // 47 - 17 = 30
  cur = cur - 32; // 30 - 32 = -2
  cur = cur + 33; // -2 + 33 = 31
  cur = cur - 21; // 31 - 21 = 10
  cur = cur - 31; // 10 - 31 = -21
  cur = cur + 10; // -21 + 10 = -11
  cur = cur - 27; // -11 - 27 = -38
  cur = cur - 18; // -38 - 18 = -56
  cur = cur + 6; // -56 + 6 = -50
  cur = cur - 5; // -50 - 5 = -55
  cur = cur + 25; // -55 + 25 = -30
  cur = cur + 34; // -30 + 34 = 4
  cur = cur - 30; // 4 - 30 = -26
  cur = cur + 3; // -26 + 3 = -23
  cur = cur - 29; // -23 - 29 = -52
  cur = cur - 26; // -52 - 26 = -78
  cur = cur - 15; // -78 - 15 = -93
  cur = cur + 14; // -93 + 14 = -79
  cur = cur - 21; // -79 - 21 = -100
  cur = cur + 16; // -100 + 16 = -84
  cur = cur + 19; // -84 + 19 = -65
  cur = cur - 3; // -65 - 3 = -68
  cur = cur - 13; // -68 - 13 = -81
  cur = cur + 30; // -81 + 30 = -51
  if -51 != cur { exit(12); }
  return cur;
}

fn check_12(cur: i64) -> i64 {
  if 0 == cur { exit(13) };
  let mut cur = cur;
  cur = cur - 24; // 47 - 24 = 23
  cur = cur - 25; // 23 - 25 = -2
  cur = cur - 22; // -2 - 22 = -24
  cur = cur - 12; // -24 - 12 = -36
  cur = cur + 9; // -36 + 9 = -27
  cur = cur - 25; // -27 - 25 = -52
  cur = cur - 32; // -52 - 32 = -84
  cur = cur - 12; // -84 - 12 = -96
  cur = cur - 5; // -96 - 5 = -101
  cur = cur + 32; // -101 + 32 = -69
  cur = cur + 6; // -69 + 6 = -63
  cur = cur - 13; // -63 - 13 = -76
  cur = cur - 9; // -76 - 9 = -85
  cur = cur - 23; // -85 - 23 = -108
  cur = cur - 11; // -108 - 11 = -119
  cur = cur + 10; // -119 + 10 = -109
  cur = cur - 12; // -109 - 12 = -121
  cur = cur - 21; // -121 - 21 = -142
  cur = cur + 14; // -142 + 14 = -128
  cur = cur - 18; // -128 - 18 = -146
  cur = cur + 10; // -146 + 10 = -136
  cur = cur - 20; // -136 - 20 = -156
  cur = cur + 10; // -156 + 10 = -146
  cur = cur - 32; // -146 - 32 = -178
  cur = cur - 28; // -178 - 28 = -206
  cur = cur - 10; // -206 - 10 = -216
  cur = cur - 18; // -216 - 18 = -234
  cur = cur - 34; // -234 - 34 = -268
  cur = cur - 15; // -268 - 15 = -283
  cur = cur - 33; // -283 - 33 = -316
  cur = cur - 15; // -316 - 15 = -331
  cur = cur - 16; // -331 - 16 = -347
  if -347 != cur { exit(13); }
  return cur;
}

fn check_13(cur: i64) -> i64 {
  if 0 == cur { exit(14) };
  let mut cur = cur;
  cur = cur + 11; // 47 + 11 = 58
  cur = cur - 26; // 58 - 26 = 32
  cur = cur + 17; // 32 + 17 = 49
  cur = cur + 29; // 49 + 29 = 78
  cur = cur + 26; // 78 + 26 = 104
  cur = cur - 15; // 104 - 15 = 89
  cur = cur - 16; // 89 - 16 = 73
  cur = cur + 28; // 73 + 28 = 101
  cur = cur - 27; // 101 - 27 = 74
  cur = cur + 8; // 74 + 8 = 82
  cur = cur + 21; // 82 + 21 = 103
  cur = cur - 3; // 103 - 3 = 100
  cur = cur - 30; // 100 - 30 = 70
  cur = cur + 6; // 70 + 6 = 76
  cur = cur + 18; // 76 + 18 = 94
  cur = cur - 17; // 94 - 17 = 77
  cur = cur + 22; // 77 + 22 = 99
  cur = cur + 8; // 99 + 8 = 107
  cur = cur + 28; // 107 + 28 = 135
  cur = cur + 23; // 135 + 23 = 158
  cur = cur + 28; // 158 + 28 = 186
  cur = cur + 14; // 186 + 14 = 200
  cur = cur - 8; // 200 - 8 = 192
  cur = cur + 25; // 192 + 25 = 217
  cur = cur + 10; // 217 + 10 = 227
  cur = cur - 7; // 227 - 7 = 220
  cur = cur - 29; // 220 - 29 = 191
  cur = cur + 34; // 191 + 34 = 225
  cur = cur + 29; // 225 + 29 = 254
  cur = cur + 32; // 254 + 32 = 286
  cur = cur + 7; // 286 + 7 = 293
  cur = cur - 21; // 293 - 21 = 272
  if 272 != cur { exit(14); }
  return cur;
}

fn check_14(cur: i64) -> i64 {
  if 0 == cur { exit(15) };
  let mut cur = cur;
  cur = cur + 13; // 98 + 13 = 111
  cur = cur + 6; // 111 + 6 = 117
  cur = cur - 28; // 117 - 28 = 89
  cur = cur + 25; // 89 + 25 = 114
  cur = cur + 16; // 114 + 16 = 130
  cur = cur + 16; // 130 + 16 = 146
  cur = cur + 21; // 146 + 21 = 167
  cur = cur + 11; // 167 + 11 = 178
  cur = cur + 8; // 178 + 8 = 186
  cur = cur + 14; // 186 + 14 = 200
  cur = cur - 14; // 200 - 14 = 186
  cur = cur + 26; // 186 + 26 = 212
  cur = cur + 5; // 212 + 5 = 217
  cur = cur + 17; // 217 + 17 = 234
  cur = cur - 28; // 234 - 28 = 206
  cur = cur + 28; // 206 + 28 = 234
  cur = cur + 7; // 234 + 7 = 241
  cur = cur - 6; // 241 - 6 = 235
  cur = cur + 14; // 235 + 14 = 249
  cur = cur - 4; // 249 - 4 = 245
  cur = cur + 9; // 245 + 9 = 254
  cur = cur + 7; // 254 + 7 = 261
  cur = cur - 7; // 261 - 7 = 254
  cur = cur - 10; // 254 - 10 = 244
  cur = cur + 17; // 244 + 17 = 261
  cur = cur - 30; // 261 - 30 = 231
  cur = cur + 14; // 231 + 14 = 245
  cur = cur - 23; // 245 - 23 = 222
  cur = cur + 22; // 222 + 22 = 244
  cur = cur - 11; // 244 - 11 = 233
  cur = cur - 12; // 233 - 12 = 221
  cur = cur - 8; // 221 - 8 = 213
  if 213 != cur { exit(15); }
  return cur;
}

fn check_15(cur: i64) -> i64 {
  if 0 == cur { exit(16) };
  let mut cur = cur;
  cur = cur + 22; // 111 + 22 = 133
  cur = cur - 21; // 133 - 21 = 112
  cur = cur + 6; // 112 + 6 = 118
  cur = cur - 31; // 118 - 31 = 87
  cur = cur + 20; // 87 + 20 = 107
  cur = cur - 20; // 107 - 20 = 87
  cur = cur - 20; // 87 - 20 = 67
  cur = cur - 12; // 67 - 12 = 55
  cur = cur + 28; // 55 + 28 = 83
  cur = cur - 28; // 83 - 28 = 55
  cur = cur + 22; // 55 + 22 = 77
  cur = cur + 32; // 77 + 32 = 109
  cur = cur - 14; // 109 - 14 = 95
  cur = cur + 28; // 95 + 28 = 123
  cur = cur + 8; // 123 + 8 = 131
  cur = cur - 20; // 131 - 20 = 111
  cur = cur - 16; // 111 - 16 = 95
  cur = cur + 28; // 95 + 28 = 123
  cur = cur - 27; // 123 - 27 = 96
  cur = cur + 6; // 96 + 6 = 102
  cur = cur + 21; // 102 + 21 = 123
  cur = cur + 17; // 123 + 17 = 140
  cur = cur + 16; // 140 + 16 = 156
  cur = cur + 19; // 156 + 19 = 175
  cur = cur + 16; // 175 + 16 = 191
  cur = cur + 34; // 191 + 34 = 225
  cur = cur + 29; // 225 + 29 = 254
  cur = cur - 31; // 254 - 31 = 223
  cur = cur + 18; // 223 + 18 = 241
  cur = cur + 20; // 241 + 20 = 261
  cur = cur - 17; // 261 - 17 = 244
  cur = cur + 24; // 244 + 24 = 268
  if 268 != cur { exit(16); }
  return cur;
}

pub fn check(got: String) -> i64 {
  let mut bytes = got.bytes();
  let mut sum: i64 = 0;
  let mut cur: i64;
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_0(cur); // 0 + 296 = 296
  sum = sum >> 3; // 296 >> 3 = 37
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_1(cur); // 37 + 76 = 113
  sum = sum >> 3; // 113 >> 3 = 14
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_2(cur); // 14 + -183 = -169
  sum = sum >> 3; // -169 >> 3 = -22
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_3(cur); // -22 + 273 = 251
  sum = sum >> 3; // 251 >> 3 = 31
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_4(cur); // 31 + 192 = 223
  sum = sum >> 3; // 223 >> 3 = 27
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_5(cur); // 27 + 194 = 221
  sum = sum >> 3; // 221 >> 3 = 27
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_6(cur); // 27 + 21 = 48
  sum = sum >> 3; // 48 >> 3 = 6
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_7(cur); // 6 + 38 = 44
  sum = sum >> 3; // 44 >> 3 = 5
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_8(cur); // 5 + 283 = 288
  sum = sum >> 3; // 288 >> 3 = 36
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_9(cur); // 36 + 81 = 117
  sum = sum >> 3; // 117 >> 3 = 14
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_10(cur); // 14 + 219 = 233
  sum = sum >> 3; // 233 >> 3 = 29
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_11(cur); // 29 + -51 = -22
  sum = sum >> 3; // -22 >> 3 = -3
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_12(cur); // -3 + -347 = -350
  sum = sum >> 3; // -350 >> 3 = -44
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_13(cur); // -44 + 272 = 228
  sum = sum >> 3; // 228 >> 3 = 28
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_14(cur); // 28 + 213 = 241
  sum = sum >> 3; // 241 >> 3 = 30
  cur = bytes.next().unwrap() as i64;
  sum = sum + check_15(cur); // 30 + 268 = 298
  sum = sum >> 3; // 298 >> 3 = 37
  if 37 != sum {exit(250)};
  return sum;
}
