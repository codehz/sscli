workflow "Build Assets" {
  on = "release"
  resolves = ["JasonEtco/upload-to-release@master"]
}

action "codehz/arch-cmake-builder@master" {
  uses = "codehz/arch-cmake-builder@master"
  args = "CC=musl-gcc"
}

action "JasonEtco/upload-to-release@master" {
  uses = "JasonEtco/upload-to-release@master"
  needs = ["codehz/arch-cmake-builder@master"]
  args = "build/sscli"
  secrets = ["GITHUB_TOKEN"]
}
