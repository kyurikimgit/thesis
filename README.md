# MSc Kyuri Kim

## Project Overview
This project evaluates various matching algorithms applied to **Vector Bin Packing with Few Small Items**.  
It implements **Monte Carlo Matching, MVV Algorithm, and Pfaffian Calculation** using the **Boost Graph Library**.

The core program, `sample_2.cpp`, is designed to solve the bin packing problem using advanced graph-based matching techniques.

---

## Requirements
This project has been tested on **Windows (MSYS2) with MinGW-w64 and Boost 1.86.0**.  
The following dependencies must be installed:
- **GCC** (MinGW-w64)
- **Boost Graph Library** (`boost_1_86_0`)
- **C++17 or later**

If Boost is not installed, follow these steps:

### Install Boost on Windows (MSYS2)
```bash
pacman -S mingw-w64-x86_64-boost
```

### Install Boost on macOS
```bash
brew install boost
```

---

## Installation and Execution

### Clone the repository
```bash
git clone https://collaborating.tuhh.de/e-11/theses/msc-kyuri-kim.git
cd thesis_project/thesiscode
```

### Compile using Makefile
```bash
make
```
This will generate `sample_2.exe`.

### Run the program
```bash
./sample_2.exe
```

### Clean up compiled files
```bash
make clean
```
This removes the `.o` object files and `sample_2.exe`.

---

## Input Format
The dataset should be in `.txt` format and must follow this structure:
```
<bin weight capacity> <bin volume capacity>
<index> <demand> <weight> <volume>
...
```

Example (`CL_01_25_01.txt`):
```
772 891
0 30 224 301
1 72 195 278
2 4 269 139
...
```

---

## Output Format
The program prints whether a perfect matching is found and measures runtime.

Example output:
```
Pfaffian: 123456
Perfect matching found!
Runtime: 0.0234 seconds
```

---

## Performance Evaluation
The runtime of the algorithms is measured using `std::chrono::high_resolution_clock`.  
To test different datasets, update the `filename` variable in `sample_2.cpp`.

---

## Troubleshooting

### Boost Library Not Found
- Ensure Boost is installed in: `C:/Users/rlarb/boost_1_86_0`
- Add the correct include path in `Makefile`:
  ```makefile
  CXXFLAGS = -I"C:/Users/rlarb/boost_1_86_0"
  ```

### Execution Error
If the program fails to run, try:
```bash
make clean
make
./sample_2.exe
```

---

## References
- **Paper:** *Tight Vector Bin Packing with Few Small Items via Fast Exact Matching in Multigraphs*
- **Boost Graph Library Documentation:** [Boost.org](https://www.boost.org)
```

---

### **How to Apply This README**
1. Open `README.md` in VSCode  
2. Copy and paste the content above  
3. Save the file with `Ctrl + S`  
4. Push the changes to GitHub:
   ```bash
   git add README.md
   git commit -m "Refined README with academic formatting"
   git push
   ```





----------





## Getting started

To make it easy for you to get started with GitLab, here's a list of recommended next steps.

Already a pro? Just edit this README.md and make it your own. Want to make it easy? [Use the template at the bottom](#editing-this-readme)!

## Add your files

- [ ] [Create](https://docs.gitlab.com/ee/user/project/repository/web_editor.html#create-a-file) or [upload](https://docs.gitlab.com/ee/user/project/repository/web_editor.html#upload-a-file) files
- [ ] [Add files using the command line](https://docs.gitlab.com/ee/gitlab-basics/add-file.html#add-a-file-using-the-command-line) or push an existing Git repository with the following command:

```
cd existing_repo
git remote add origin https://collaborating.tuhh.de/e-11/theses/msc-kyuri-kim.git
git branch -M main
git push -uf origin main
```

## Integrate with your tools

- [ ] [Set up project integrations](https://collaborating.tuhh.de/e-11/theses/msc-kyuri-kim/-/settings/integrations)

## Collaborate with your team

- [ ] [Invite team members and collaborators](https://docs.gitlab.com/ee/user/project/members/)
- [ ] [Create a new merge request](https://docs.gitlab.com/ee/user/project/merge_requests/creating_merge_requests.html)
- [ ] [Automatically close issues from merge requests](https://docs.gitlab.com/ee/user/project/issues/managing_issues.html#closing-issues-automatically)
- [ ] [Enable merge request approvals](https://docs.gitlab.com/ee/user/project/merge_requests/approvals/)
- [ ] [Set auto-merge](https://docs.gitlab.com/ee/user/project/merge_requests/merge_when_pipeline_succeeds.html)

## Test and Deploy

Use the built-in continuous integration in GitLab.

- [ ] [Get started with GitLab CI/CD](https://docs.gitlab.com/ee/ci/quick_start/index.html)
- [ ] [Analyze your code for known vulnerabilities with Static Application Security Testing (SAST)](https://docs.gitlab.com/ee/user/application_security/sast/)
- [ ] [Deploy to Kubernetes, Amazon EC2, or Amazon ECS using Auto Deploy](https://docs.gitlab.com/ee/topics/autodevops/requirements.html)
- [ ] [Use pull-based deployments for improved Kubernetes management](https://docs.gitlab.com/ee/user/clusters/agent/)
- [ ] [Set up protected environments](https://docs.gitlab.com/ee/ci/environments/protected_environments.html)

***

# Editing this README

When you're ready to make this README your own, just edit this file and use the handy template below (or feel free to structure it however you want - this is just a starting point!). Thanks to [makeareadme.com](https://www.makeareadme.com/) for this template.

## Suggestions for a good README

Every project is different, so consider which of these sections apply to yours. The sections used in the template are suggestions for most open source projects. Also keep in mind that while a README can be too long and detailed, too long is better than too short. If you think your README is too long, consider utilizing another form of documentation rather than cutting out information.

## Name
Choose a self-explaining name for your project.

## Description
Let people know what your project can do specifically. Provide context and add a link to any reference visitors might be unfamiliar with. A list of Features or a Background subsection can also be added here. If there are alternatives to your project, this is a good place to list differentiating factors.

## Badges
On some READMEs, you may see small images that convey metadata, such as whether or not all the tests are passing for the project. You can use Shields to add some to your README. Many services also have instructions for adding a badge.

## Visuals
Depending on what you are making, it can be a good idea to include screenshots or even a video (you'll frequently see GIFs rather than actual videos). Tools like ttygif can help, but check out Asciinema for a more sophisticated method.

## Installation
Within a particular ecosystem, there may be a common way of installing things, such as using Yarn, NuGet, or Homebrew. However, consider the possibility that whoever is reading your README is a novice and would like more guidance. Listing specific steps helps remove ambiguity and gets people to using your project as quickly as possible. If it only runs in a specific context like a particular programming language version or operating system or has dependencies that have to be installed manually, also add a Requirements subsection.

## Usage
Use examples liberally, and show the expected output if you can. It's helpful to have inline the smallest example of usage that you can demonstrate, while providing links to more sophisticated examples if they are too long to reasonably include in the README.

## Support
Tell people where they can go to for help. It can be any combination of an issue tracker, a chat room, an email address, etc.

## Roadmap
If you have ideas for releases in the future, it is a good idea to list them in the README.

## Contributing
State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood that the changes inadvertently break something. Having instructions for running tests is especially helpful if it requires external setup, such as starting a Selenium server for testing in a browser.

## Authors and acknowledgment
Show your appreciation to those who have contributed to the project.

## License
For open source projects, say how it is licensed.

## Project status
If you have run out of energy or time for your project, put a note at the top of the README saying that development has slowed down or stopped completely. Someone may choose to fork your project or volunteer to step in as a maintainer or owner, allowing your project to keep going. You can also make an explicit request for maintainers.
