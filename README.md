# SharpColdClear
C# binding for [ColdClear](https://github.com/MinusKelvin/cold-clear.git)

First, build cold clear
```
git clone https://github.com/MinusKelvin/cold-clear.git
cd cold-clear
cargo build --release
```
Copy `c-api/coldclear.h`, `target/release/cold_clear.dll`, `target/release/cold_clear.dll.lib` to root folder.

Then open SharpColdClear.sln in VisualStudio.