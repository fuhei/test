fn main() {
   let age:u8 = 255;

   // u8 只能存储 0 to 255 
   let weight:u8 = 256;   // 溢出值为 0
   let height:u8 = 257;   // 溢出值为 1
   let score:u8 = 258;    // 溢出值为 2

   println!("age is {} ",age);
   println!("weight is {}",weight);
   println!("height is {}",height);
   println!("score is {}",score);
}
