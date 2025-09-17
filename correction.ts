function main() {
  let hoge = 10;
  while (hoge < 10) {
    for (let i = 0; i < 10; i++) {
      if (i == 3) {
        continue;
      }
      hoge++;
    }
  }
  return hoge;
}

console.log(main());
