/*
  ISC License

  Copyright (c) 2023, Antonio SJ Musumeci <trapexit@spawn.link>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "CLI11.hpp"
#include "fmt.hpp"

#include <cstdint>
#include <filesystem>

#define MAJOR 1
#define MINOR 0
#define PATCH 0

enum
  {
    DIVIDE_BY_2_MINUS_1,
    DIVIDE_BY_2_PLUS_1
  };


static
bool
is_power_of_2(uint32_t n_)
{
  return (n_ && !(n_ & (n_ - 1)));
}

static
std::uint32_t
uint32_log2(uint32_t n_)
{
  uint32_t logn;

  logn = 0;
  for(uint32_t bit = 0, pow = 1; bit < 31; bit++)
    {
      if(n_ == pow)
        logn = bit;
      pow <<= 1;
    }

  return logn;
}

static
void
div2m1(FILE     *file_,
       uint32_t  logd_,
       uint32_t  logmsb_)
{
  /* Output instructions to do division by 2^n - 1 */
  fmt::print(file_,"     MOV     rv.q, rv.q, LSR #1;\n");
  while(logd_ < 32)
    {
      fmt::print(file_,"     ADD     rv.q, rv.q, rv.q, LSR #{};\n",logd_);
      logd_ <<= 1;
    }
  fmt::print(file_,"     MOV     rv.q, rv.q, LSR #{};\n",logmsb_ - 1);
}

void
div2p1(FILE     *file_,
       uint32_t  logd_,
       uint32_t  logmsb_)
{
  /* Output instructions to do division by 2^n + 1 */
  fmt::print(file_,"     SUB     rv.q, rv.q, rv.q, LSR #{};\n",logd_);
  while(logd_ < 16)
    {
      logd_ <<= 1;
      fmt::print(file_,"     ADD     rv.q, rv.q, rv.q, LSR #{};\n",logd_);
    }
  fmt::print(file_,"     MOV     rv.q, rv.q, LSR #{};\n",logmsb_);
}

static
void
load_divisor(FILE     *file_,
             uint32_t  type_,
             uint32_t  lsb_,
             uint32_t  msb_ )
{
  /* Constant is too big to be used as an immediate constant, */
  /* so load it into register.                                */
  fmt::print(file_,"     MOV     divisor, #{};\n",msb_);
  switch(type_)
    {
    case DIVIDE_BY_2_MINUS_1:
      fmt::print(file_,"     SUB     divisor, divisor, #{};\n",lsb_);
      break;
    case DIVIDE_BY_2_PLUS_1:
      fmt::print(file_,"     ADD     divisor, divisor, #{};\n",lsb_);
      break;
    default:
      fmt::print(stderr,"Internal error");
      break;
    }
}

static
void
generate_div_type(FILE *file_)
{
  fmt::print(file_,
             "#ifndef DIV_T_DEFINED\n"
             "#define DIV_T_DEFINED\n"
             "typedef struct div_t {{ unsigned q; unsigned r; }} div_t;\n"
             "#endif\n\n");
}

static
void
generate_code_q(FILE     *file_,
                uint32_t  type_,
                uint32_t  n_,
                uint32_t  lsb_,
                uint32_t  msb_)
{
  bool     larger_than_byte;
  uint32_t loglsb;
  uint32_t logmsb;

  loglsb = uint32_log2(lsb_);
  logmsb = uint32_log2(msb_);
  larger_than_byte = false; // ((n_ >> loglsb) > 255);

  fmt::print(file_,
             "static\n"
             "__inline\n"
             "__pure\n"
             "unsigned\n"
             "div_by_{}_q(register unsigned d_)\n"
             "{{\n"
             "  register div_t rv;\n"
             "  register unsigned tmp;\n"
             "\n"
             "  rv.q = d_;\n"
             "\n"
             "  __asm\n"
             "  {{\n"
             ,
             n_);

  if(larger_than_byte)
    {
      load_divisor(file_,type_,lsb_,msb_);
      fmt::print(file_,"     SUB     rv.r, rv.q, divisor;\n");
    }
  else
    {
      fmt::print(file_,"     SUB     rv.r, rv.q, #{};\n",n_);
    }

  /* 1/n as a binary number consists of a simple repeating pattern     */
  /* The multiply by 1/n is expanded as a sequence of ARM instructions */
  /* (there is a rounding error which must be corrected later)         */
  switch(type_)
    {
    case DIVIDE_BY_2_MINUS_1:
      div2m1(file_,logmsb - loglsb,logmsb);
      /* Now do multiply-by-n */
      fmt::print(file_,"     RSB     tmp, rv.q, rv.q, ASL #{};\n",logmsb - loglsb);
      break;
    case DIVIDE_BY_2_PLUS_1:
      div2p1(file_,logmsb - loglsb,logmsb);
      /* Now do multiply-by-n */
      fmt::print(file_,"     ADD     tmp, rv.q, rv.q, ASL #{};\n",logmsb - loglsb);
      break;
    default:
      fmt::print(stderr,"Internal error");
      break;
    }

  /* Subtract from adjusted original to obtain remainder */
  fmt::print(file_,"     SUBS    rv.r, rv.r, tmp, ASL #{};\n",loglsb);

  /* Apply corrections */
  fmt::print(file_,"     ADDPL   rv.q, rv.q, #1;\n");
  // if(larger_than_byte)
  //   fmt::print("     ADDMI   rv.r, rv.r, divisor;\n");
  // else
  //   fmt::print("     ADDMI   rv.r, rv.r, #{};\n",n_);

  /* Additional test required for divide-by-3, as result could be  */
  /* off by 2 lsb due to accumulated rounding errors.              */
  if(n_ == 3)
    {
      fmt::print(file_,"     CMP     rv.r, #3;\n");
      fmt::print(file_,"     ADDGE   rv.q, rv.q, #1;\n");
      //fmt::print("     SUBGE   rv.r, rv.r, #3;\n");
    }

  fmt::print(file_,
             "  }}\n"
             "\n"
             "  return rv.q;\n"
             "}}\n");
}

static
void
generate_code_r(FILE     *file_,
                uint32_t  type_,
                uint32_t  n_,
                uint32_t  lsb_,
                uint32_t  msb_)
{
  bool     larger_than_byte;
  uint32_t loglsb;
  uint32_t logmsb;

  loglsb = uint32_log2(lsb_);
  logmsb = uint32_log2(msb_);
  larger_than_byte = false; // ((n_ >> loglsb) > 255);

  fmt::print(file_,
             "static\n"
             "__inline\n"
             "__pure\n"
             "unsigned\n"
             "div_by_{}_r(register unsigned d_)\n"
             "{{\n"
             "  register div_t rv;\n"
             "  register unsigned tmp;\n"
             "\n"
             "  rv.q = d_;\n"
             "\n"
             "  __asm\n"
             "  {{\n"
             ,
             n_);

  if(larger_than_byte)
    {
      load_divisor(file_,type_,lsb_,msb_);
      fmt::print(file_,"     SUB     rv.r, rv.q, divisor;\n");
    }
  else
    {
      fmt::print(file_,"     SUB     rv.r, rv.q, #{};\n",n_);
    }

  /* 1/n as a binary number consists of a simple repeating pattern     */
  /* The multiply by 1/n is expanded as a sequence of ARM instructions */
  /* (there is a rounding error which must be corrected later)         */
  switch(type_)
    {
    case DIVIDE_BY_2_MINUS_1:
      div2m1(file_,logmsb - loglsb,logmsb);
      /* Now do multiply-by-n */
      fmt::print(file_,"     RSB     tmp, rv.q, rv.q, ASL #{};\n",logmsb - loglsb);
      break;
    case DIVIDE_BY_2_PLUS_1:
      div2p1(file_,logmsb - loglsb,logmsb);
      /* Now do multiply-by-n */
      fmt::print(file_,"     ADD     tmp, rv.q, rv.q, ASL #{};\n",logmsb - loglsb);
      break;
    default:
      fmt::print(stderr,"Internal error");
      break;
    }

  /* Subtract from adjusted original to obtain remainder */
  fmt::print(file_,"     SUBS    rv.r, rv.r, tmp, ASL #{};\n",loglsb);

  /* Apply corrections */
  //fmt::print("     ADDPL   rv.q, rv.q, #1;\n");
  if(larger_than_byte)
    fmt::print(file_,"     ADDMI   rv.r, rv.r, divisor;\n");
  else
    fmt::print(file_,"     ADDMI   rv.r, rv.r, #{};\n",n_);

  /* Additional test required for divide-by-3, as result could be  */
  /* off by 2 lsb due to accumulated rounding errors.              */
  if(n_ == 3)
    {
      fmt::print(file_,"     CMP     rv.r, #3;\n");
      //fmt::print("     ADDGE   rv.q, rv.q, #1;\n");
      fmt::print(file_,"     SUBGE   rv.r, rv.r, #3;\n");
    }

  fmt::print(file_,
             "  }}\n"
             "\n"
             "  return rv.r;\n"
             "}}\n");
}

static
void
generate_code_qr(FILE     *file_,
                 uint32_t  type_,
                 uint32_t  n_,
                 uint32_t  lsb_,
                 uint32_t  msb_)
{
  bool     larger_than_byte;
  uint32_t loglsb;
  uint32_t logmsb;

  loglsb = uint32_log2(lsb_);
  logmsb = uint32_log2(msb_);
  larger_than_byte = false; // ((n_ >> loglsb) > 255);

  fmt::print(file_,
             "static\n"
             "__inline\n"
             "__pure\n"
             "__value_in_regs\n"
             "div_t\n"
             "div_by_{}_qr(register unsigned d_)\n"
             "{{\n"
             "  register div_t rv;\n"
             "  register unsigned tmp;\n"
             "\n"
             "  rv.q = d_;\n"
             "\n"
             "  __asm\n"
             "  {{\n"
             ,
             n_);

  if(larger_than_byte)
    {
      load_divisor(file_,type_,lsb_,msb_);
      fmt::print(file_,"     SUB     rv.r, rv.q, divisor;\n");
    }
  else
    {
      fmt::print(file_,"     SUB     rv.r, rv.q, #{};\n",n_);
    }

  /* 1/n as a binary number consists of a simple repeating pattern     */
  /* The multiply by 1/n is expanded as a sequence of ARM instructions */
  /* (there is a rounding error which must be corrected later)         */
  switch(type_)
    {
    case DIVIDE_BY_2_MINUS_1:
      div2m1(file_,logmsb - loglsb,logmsb);
      /* Now do multiply-by-n */
      fmt::print(file_,"     RSB     tmp, rv.q, rv.q, ASL #{};\n",logmsb - loglsb);
      break;
    case DIVIDE_BY_2_PLUS_1:
      div2p1(file_,logmsb - loglsb,logmsb);
      /* Now do multiply-by-n */
      fmt::print(file_,"     ADD     tmp, rv.q, rv.q, ASL #{};\n",logmsb - loglsb);
      break;
    default:
      fmt::print(stderr,"Internal error");
      break;
    }

  /* Subtract from adjusted original to obtain remainder */
  fmt::print(file_,"     SUBS    rv.r, rv.r, tmp, ASL #{};\n",loglsb);

  /* Apply corrections */
  fmt::print(file_,"     ADDPL   rv.q, rv.q, #1;\n");
  if(larger_than_byte)
    fmt::print(file_,"     ADDMI   rv.r, rv.r, divisor;\n");
  else
    fmt::print(file_,"     ADDMI   rv.r, rv.r, #{};\n",n_);

  /* Additional test required for divide-by-3, as result could be  */
  /* off by 2 lsb due to accumulated rounding errors.              */
  if(n_ == 3)
    {
      fmt::print(file_,"     CMP     rv.r, #3;\n");
      fmt::print(file_,"     ADDGE   rv.q, rv.q, #1;\n");
      fmt::print(file_,"     SUBGE   rv.r, rv.r, #3;\n");
    }

  fmt::print(file_,
             "  }}\n"
             "\n"
             "  return rv;\n"
             "}}\n");
}

void
generate_code_for_divisor_plus_lsb(FILE     *file_,
                                   int       divisor_,
                                   unsigned  lsb_)
{
  generate_code_qr(file_,DIVIDE_BY_2_MINUS_1,divisor_,lsb_,divisor_ + lsb_);
  fmt::print(file_,"\n");
  generate_code_q(file_,DIVIDE_BY_2_MINUS_1,divisor_,lsb_,divisor_ + lsb_);
  fmt::print(file_,"\n");
  generate_code_r(file_,DIVIDE_BY_2_MINUS_1,divisor_,lsb_,divisor_ + lsb_);
}

void
generate_code_for_divisor_minus_lsb(FILE     *file_,
                                    int       divisor_,
                                    unsigned  lsb_)
{
  generate_code_qr(file_,DIVIDE_BY_2_PLUS_1,divisor_,lsb_,divisor_ - lsb_);
  fmt::print(file_,"\n");
  generate_code_q(file_,DIVIDE_BY_2_PLUS_1,divisor_,lsb_,divisor_ - lsb_);
  fmt::print(file_,"\n");
  generate_code_r(file_,DIVIDE_BY_2_PLUS_1,divisor_,lsb_,divisor_ - lsb_);
}

static
void
generate_code(std::filesystem::path output_path_,
              int                   divisor_,
              unsigned              lsb_)
{
  FILE *file;

  if(output_path_.empty())
    output_path_ = fmt::format("div_by_{}.h",divisor_);
  else if(std::filesystem::is_directory(output_path_))
    output_path_ /= fmt::format("div_by_{}.h",divisor_);

  file = fopen(output_path_.string().c_str(),"w");

  fmt::print(file,
             "#ifndef DIV_BY_{0}_INCLUDED\n"
             "#define DIV_BY_{0}_INCLUDED\n\n",divisor_);
  generate_div_type(file);

  if(is_power_of_2(divisor_ + lsb_))
    generate_code_for_divisor_plus_lsb(file,divisor_,lsb_);
  else if(is_power_of_2(divisor_ - lsb_))
    generate_code_for_divisor_minus_lsb(file,divisor_,lsb_);

  fmt::print(file,"\n#endif\n");

  fclose(file);

  fmt::print("header file written: {}\n",output_path_);
}

static
void
print_version_and_exit()
{
  fmt::print("armdbccg v{}.{}.{}\n\n"
             "https://github.com/trapexit/armdbccg\n"
             "https://github.com/trapexit/support\n\n"
             "ISC License (ISC)\n\n"
             "Copyright 2023, Antonio SJ Musumeci <trapexit@spawn.link>\n\n"
             "Permission to use, copy, modify, and/or distribute this software for\n"
             "any purpose with or without fee is hereby granted, provided that the\n"
             "above copyright notice and this permission notice appear in all\n"
             "copies.\n\n"
             "THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL\n"
             "WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED\n"
             "WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE\n"
             "AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL\n"
             "DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR\n"
             "PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER\n"
             "TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR\n"
             "PERFORMANCE OF THIS SOFTWARE.\n\n",
             MAJOR,MINOR,PATCH);
  exit(0);
}

static
void
print_divisor_usage_and_exit()
{
  fmt::print("divisor is required\n"
             "Run with --help for more information.\n");
  exit(0);
}

static
void
set_locale()
{
  try
    {
      std::locale::global(std::locale(""));
    }
  catch(const std::runtime_error &e)
    {
      std::locale::global(std::locale("C"));
    }
}

int
main(int    argc_,
     char **argv_)
{
  int divisor = 0;
  bool print_version = false;
  std::filesystem::path output_path;
  CLI::App app("armdbccg: optimal ARM divide-by-constant code generator");

  set_locale();

  app.add_flag("-v,--version",print_version)
    ->description("print version and exit");
  app.add_option("-o,--output-path",output_path)
    ->description("dirpath or filepath for output")
    ->type_name("PATH");
  app.add_option("divisor",divisor)
    ->description("constant divisor");

  try
    {
      app.parse(argc_,argv_);

      if(print_version)
        print_version_and_exit();
      if(divisor == 0)
        print_divisor_usage_and_exit();

      if(divisor <= 1)
        {
          fmt::print(stderr,"{} <= 1: doesn't make sense to do or not possible\n",divisor);
        }
      else
        {
          unsigned lsb = 1;
          unsigned shift = 0;

          while((divisor & lsb) == 0)
            {
              lsb <<= 1;
              shift++;
            }

          if(is_power_of_2(divisor))
            fmt::print("use '>> {}'\n",shift);
          else if(is_power_of_2(divisor + lsb) || is_power_of_2(divisor - lsb))
            generate_code(output_path,divisor,lsb);
          else
            fmt::print("Unfortunately, {} is not (2^n-2^m) or (2^n+2^m)\n",divisor);
        }
    }
  catch(const CLI::ParseError &e_)
    {
      return app.exit(e_);
    }
  catch(const std::system_error &e_)
    {
      fmt::print("{} ({})\n",e_.what(),e_.code().message());
      return 1;
    }
  catch(const std::runtime_error &e_)
    {
      fmt::print("{}\n",e_.what());
      return 1;
    }

  return 0;
}
