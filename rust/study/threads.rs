//https://www.twle.cn/c/yufei/rust/rust-basic-concurrency.html

use std::thread;           // 导入线程模块
use std::time::Duration;   // 导入时间模块

fn main() {
   //创建一个新线程
   thread::spawn(|| {
      for i in 1..10 {
         println!("hi number {} from the spawned thread!", i);
         thread::sleep(Duration::from_millis(1));
      }
   });
   // 主线程要执行的代码
   for i in 1..5 {
      println!("hi number {} from the main thread!", i);
      thread::sleep(Duration::from_millis(1));
   }
}
