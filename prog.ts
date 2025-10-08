function hoge(arg: integer) {
  if (arg == 100) {
    return arg;
  }
  return hoge(arg + 1);
}

function entry() {}
