function hoge() {
  let hoge = 100;
  {
    let some = 200;
  }
  let hoge = some;
}
