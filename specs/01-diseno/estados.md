# Estados del Autómata
Se describen los **estados** utilizados por el **AF** del analizador léxico del compilador.  

---

## Tabla de Estados del Autómata

| Estado | Descripción |
|--------|-------------|
| **0**  | Estado inicial |
| **1**  | Posible ID o palabra reservada |
| **2**  | Digito |
| **3**  | Pasar de entero a real con `.` |
| **4**  | Inicio de cadena `"` |
| **5**  | Fin de cadena `"` |
| **6**  | `{` |
| **7**  | `}` |
| **8**  | `(` |
| **9**  | `)` |
| **10** | `>` |
| **11** | `>=` |
| **12** | `<` |
| **13** | `!=` |
| **14** | `<=` |
| **15** | `&` |
| **16** | `&&` |
| **17** | `\|` |
| **18** | `\|\|`|
| **19** | Posible `+` o concatenacion |
| **20** | `++` |
| **21** | `+=` |
| **22** | `-` |
| **23** | `--` |
| **24** | `-=` |
| **25** | `=` |
| **26** | `==` |
| **27** | `;` |
| **28** | `,` |
| **29** | `/` |
| **30** | `/=` |
| **31** | Inicio comentario `/*` |
| **32** | Continuar o terminar comentario |
| **33** | `*` |
| **34** | `*=` |

