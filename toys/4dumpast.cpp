auto hoge(int arg) {
  if (arg == 100) {
    return 100;
  }
  return hoge(arg + 1);
}

int main() {}
