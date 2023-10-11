#include <ruby.h>
#include <valgrind/valgrind.h>
#include <filesystem>
#include <optional>
#include <ranges>
#include <algorithm>
#include <vector>
#include <numeric>

namespace fs = std::filesystem;
namespace ranges = std::ranges;
namespace views = std::ranges::views;


static constexpr const  std::string_view default_rel_script_path("./test/debug.rb");

static const fs::path exe_path(fs::canonical(fs::path("/proc/self/exe")));
static const fs::path exe_dir(exe_path.parent_path());
//static const fs::path root_dir(fs::canonical(exe_path / "../..")); // technically, this should probably recurse to nearest Gemfile

static int state = 0;

std::optional<fs::path> find_closest(const fs::path &dir, const fs::path &rel_path) {
    auto has_path = [&rel_path](const fs::path &dir) { 
      return fs::exists(dir / rel_path); 
    };

    std::vector<fs::path> possible_dirs;
    std::partial_sum(dir.begin(), dir.end(), std::back_inserter(possible_dirs), std::divides<fs::path>{});
    ranges::viewable_range auto result = possible_dirs | views::reverse | views::filter(has_path) | views::take(1);
    if (ranges::empty(result)) return {};
    return result.front();
}

void check_state() { if (state) exit(ruby_cleanup(state)); }

struct Args {
  char **argv = nullptr;
  int argc = 0;

  using PairType = std::pair<std::size_t, char**>;
  Args() = default;

  template <typename T = std::string, template<typename> class Range_T = std::vector> requires std::is_convertible_v<T, std::string>
  Args(const Range_T<T> &range) { // NOLINT(*-explicit-constructor)
      argc = std::distance(range.begin(), range.end());
      argv = new char *[argc]{nullptr};

      for (std::size_t i = 0; i < argc; ++i) {
          const std::string &src(range[i]);
          char *&dst = argv[i];

          dst = new char[src.size() + 1]{'\0'};
          std::copy(src.cbegin(), src.cend(), &dst[0]);
      }
  }
  
  template <typename T, size_t N> requires (N > 0)
  Args(const T (&list)[N]) : Args(std::vector(&list[0], &list[N])) {} // NOLINT(*-explicit-constructor)

  template <typename T>
  Args(std::size_t size, const T (&list)[]) : Args(std::vector(&list[0], &list[size])) {}

  ~Args() {
    for (auto i = 0; i < argc; ++i) delete argv[i];
    delete[] argv;
  }

  PairType pair() { return { argc, argv }; }
};

int main(int argc, char* argv[]) {
//    VALGRIND_CLO_CHANGE("--leak-check=no");
    const auto root_dir = find_closest(exe_dir, "Gemfile").value();
    std::filesystem::current_path(root_dir);

    std::string rel_script_path(default_rel_script_path);
    if (argc < 1 || argc > 2)
      throw std::invalid_argument("Must invoke as ./debug-wrap [rel_script_path]");
    if (argc == 2) rel_script_path = std::string(argv[1]);
    rel_script_path = fs::relative(rel_script_path, root_dir).string();
    Args args({ "ruby", "--", rel_script_path });

    ruby_init();
    void* node = ruby_options(args.argc, args.argv);
    const int setup = ruby_executable_node(node, &state);
    check_state();
    assert(setup);
    ruby_init_loadpath();

//    rb_eval_string_protect("puts 'Hello, world!'", &state);
//    check_state();
    VALGRIND_PRINTF_BACKTRACE("Enabling leak checking now.\n");
    VALGRIND_CLO_CHANGE("--leak-check=full");
    state = ruby_exec_node(node);
    check_state();

    return ruby_cleanup(state);
}
