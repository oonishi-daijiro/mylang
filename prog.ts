{
  let hoge = 0;
  for (let i = 0; i < 10; i++) {
    for (let j = 0; j < 10; j++) {
      if (j == 3) {
        break;
      }
      hoge++;
    }
    if (i == 2) {
      continue;
    } else if (i == 3) {
      break;
    }
    hoge++;
  }
  return hoge;
}
