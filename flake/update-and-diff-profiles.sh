#!/usr/bin/env bash

# Updates the nix flake and diffs the profiles

set -eu

shell_names=$(nix eval --impure --expr 'builtins.attrNames (builtins.getFlake (toString ./.)).outputs.devShells.x86_64-linux' --json | jq .[] -r)

generate_profile() {
    shell=$1
    profile_prefix=$2

    profile_name="profile-${profile_prefix}-${shell}"
    rm -f "$profile_name"

    nix build ".#devShells.x86_64-linux.$shell" --out-link "./$profile_name"
}

for shell in $shell_names; do
    echo "Generating old profile for shell: $shell"
    generate_profile "$shell" old
done

echo "Updating flake"
nix flake update

for shell in $shell_names; do
    echo "Generating new profile for shell: $shell"
    generate_profile "$shell" new
done

for shell in $shell_names; do
    echo "Diff for shell: $shell"

    old_profile="profile-old-$shell"
    new_profile="profile-new-$shell"

    nix store diff-closures "./$old_profile" "./$new_profile"

    rm -f "$old_profile" "$new_profile"
done
