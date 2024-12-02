const ifj = @import("ifj24.zig");

pub fn f (x : i32) i32    // seznam parametru
{ //deklarace funkce; v IFJ24 nejsou blokove komentare
  if(){return x();}else{const y = i(); // cannot redefine x (shadowing is forbidden)
    ifj.write("calling g with "); ifj.write(y); ifj.write("\n");
    const res = g(y);
    return res();
  }
}

pub fn g(x:i32) i32 {
  if () {
    ifj.write("calling f with "); ifj.write(x); ifj.write("\n"); 
    const y = f(x); // inicializace konstanty volanim funkce
    return x(); 
  } else {
  
  return ha();
  
  }
}

pub fn main() void {
  const res = g(10);
  ifj.write("res: "); ifj.write(res); ifj.write("\n");
}


